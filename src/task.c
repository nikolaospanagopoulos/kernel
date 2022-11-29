#include "task.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "status.h"
// current task
struct task *currentTask = 0;

// LL
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
  task->process = process;
  return 0;
}
