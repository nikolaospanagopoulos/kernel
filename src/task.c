#include "task.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "paging.h"
#include "status.h"
#include "string.h"
// current task
struct task *currentTask = 0;
struct task *taskTail = 0;
struct task *taskHead = 0;

struct task *taskCurrent() {
  return currentTask;
}

struct task *taskNew(struct process *process) {
  int res = 0;
  struct task *task = kzalloc(sizeof(struct task));

  if (!task) {
    goto out;
  }

  res = taskInit(task, process);
  if (res != ALL_OK) {
    goto out;
  }

  if (taskHead == 0) {
    taskHead = task;
    taskTail = task;
    currentTask = task;
    goto out;
  }
  taskTail->next = task;
  task->previous = taskTail;
  taskTail = task;
out:
  if (ISERR(res)) {
    taskFree(task);
    return ERROR(res);
  }
  return task;
}
struct task *taskGetNext() {
  if (!currentTask->next) {
    return taskHead;
  }
  return currentTask->next;
}
static void taskListRemove(struct task *task) {
  if (task->previous) {
    task->previous->next = task->next;
  }
  if (task == taskHead) {
    taskHead = task->next;
  }
  if (task == taskTail) {
    taskTail = task->previous;
  }
  if (task == currentTask) {
    currentTask = taskGetNext();
  }
}

int taskFree(struct task *task) {

  pagingFree4gb(task->pageDirectory);
  taskListRemove(task);
  kfree(task);
  return 0;
}
int taskInit(struct task *task, struct process *process) {
  memset(task, 0, sizeof(struct task));
  task->pageDirectory =
      pagingNew4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  if (!task->pageDirectory) {
    return -EIO;
  }

  task->registers.ip =
      PROGRAM_VIRTUAL_ADDRESS; // place of execution before last interrupt

  task->registers.ss = USER_DATA_SEGMENT;
  task->registers.esp = PROGRAM_VIRTUAL_STACK_ADDRESS_START;
  task->registers.cs = USER_CODE_SEGMENT;
  task->process = process;
  return 0;
}
int taskSwitch(struct task *task) {
  currentTask = task;
  pagingSwitch(task->pageDirectory);
  return 0;
}

int taskPage() {
  userRegisters();
  taskSwitch(currentTask);
  return 0;
}

void runFirstEverTask() {
  if (!currentTask) {
    panic("no current task");
  }
  taskSwitch(taskHead);
  taskReturn(&taskHead->registers);
}

void taskSaveState(struct task *task, struct interruptFrame *frame) {

  task->registers.ip = frame->ip;
  task->registers.cs = frame->cs;
  task->registers.flags = frame->flags;
  task->registers.esp = frame->esp;
  task->registers.ss = frame->ss;
  task->registers.eax = frame->eax;
  task->registers.ebp = frame->ebp;
  task->registers.ebx = frame->ebx;
  task->registers.ecx = frame->ecx;
  task->registers.edi = frame->edi;
  task->registers.edx = frame->edx;
  task->registers.esi = frame->esi;
}

void taskSaveCurrentState(struct interruptFrame *frame) {
  if (!taskCurrent()) {
    panic("no task to save \n");
  }

  struct task *task = taskCurrent();

  taskSaveState(task, frame);
}
int copyStringFromTask(struct task *task, void *virtualP, void *phys, int max) {
  if (max >= PAGING_PAGE_SIZE) {
    return -EINVARG;
  }
  int res = 0;

  char *tmp = kzalloc(max);

  if (!tmp) {
    res = -ENOMEM;
    goto out;
  }

  struct paging4gbChunk *taskDirectory = task->pageDirectory;
  uint32_t oldEntry = pagingGet(taskDirectory->directoryEntry, tmp);
  pagingMap(taskDirectory, virtualP, phys,
            PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  pagingSwitch(task->pageDirectory);
  strncpy(tmp, virtualP, max);
  kernelPage();

  res = pagingSet(taskDirectory->directoryEntry, tmp, oldEntry);

  if (res < 0) {
    res = -EIO;
    goto outFree;
  }

  strncpy(phys, tmp, max);

outFree:
  kfree(tmp);

out:
  return res;
}

int taskPageTask(struct task *task) {
  userRegisters();
  pagingSwitch(task->pageDirectory);
  return 0;
}
void *taskGetTaskItem(struct task *task, int index) {
  void *result = 0;
  uint32_t *spPtr = (uint32_t *)task->registers.esp;
  // switch to the given tasks page (not the current one)

  taskPageTask(task);

  result = (void *)spPtr[index];

  // switch back to kernel
  kernelPage();
  return result;
}
