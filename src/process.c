#include "process.h"
#include "config.h"
#include "file.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "status.h"
#include "string.h"
#include "task.h"

// the current process
struct process *currentProcess = 0;
// all the loaded processes
static struct process *processes[MAX_PROCESSES] = {};

static void processInit(struct process *process) {
  memset(process, 0, sizeof(struct process));
}

struct process *processCurrent() {
  return currentProcess;
}

struct process *processGet(int index) {
  if (index < 0 || index >= MAX_PROCESSES) {
    return NULL;
  }
  return processes[index];
}
static int processLoadBinary(const char *filename, struct process *process) {
  int res = 0;

  int fd = fopen(filename, "r");

  if (!fd) {
    res = -EIO;
    goto out;
  }
  struct fileStat stat;
  res = fstat(fd, &stat);
  if (res != ALL_OK) {
    goto out;
  }

  void *programDataPtr = kzalloc(stat.filesize);
  if (!programDataPtr) {
    res = -ENOMEM;
    goto out;
  }

  if (fread(programDataPtr, stat.filesize, 1, fd) != 1) {

    res = -EIO;
    goto out;
  }
  process->ptr = programDataPtr;
  process->size = stat.filesize;
out:
  fclose(fd);
  return res;
}
static int processLoadData(const char *filename, struct process *process) {
  int res = 0;

  res = processLoadBinary(filename, process);

  return res;
}
int processMapBinary(struct process *process) {
  int res = 0;

  pagingMapTo(process->task->pageDirectory->directoryEntry,
              (void *)PROGRAM_VIRTUAL_ADDRESS, process->ptr,
              pagingAlignAddress(process->ptr + process->size),
              PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);

  return res;
}
int processMapMemory(struct process *process) {
  int res = 0;
  res = processMapBinary(process);
  return res;
}

int processGetFreeSlot() {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i] == 0) {
      return i;
    }
  }
  return -EISTKN;
}

int processLoad(const char *filename, struct process **process) {
  int res = 0;
  int processSlot = processGetFreeSlot();
  if (processSlot < 0) {
    res = -EISTKN;
    goto out;
  }
  res = processLoadForSlot(filename, process, processSlot);
out:
  return res;
}

int processLoadForSlot(const char *filename, struct process **process,
                       int processSlot) {
  int res = 0;
  struct task *task = 0;
  struct process *_process;
  void *programStackPtr = 0;
  if (processGet(processSlot) != 0) {
    res = -EISTKN;
    goto out;
  }
  _process = kzalloc(sizeof(struct process));
  if (!_process) {
    res = -ENOMEM;
    goto out;
  }
  processInit(_process);
  res = processLoadData(filename, _process);
  if (res < 0) {
    goto out;
  }
  programStackPtr = kzalloc(PROGRAM_STACK_SIZE);
  if (!programStackPtr) {
    res = -ENOMEM;
    goto out;
  }
  strncpy(_process->filename, filename, sizeof(_process->filename));
  _process->stack = programStackPtr;
  _process->id = processSlot;
  // create task
  task = taskNew(_process);
  if (ERROR_I(task) == 0) {
    res = ERROR_I(task);
  }
  _process->task = task;
  res = processMapMemory(_process);
  if (res < 0) {
    goto out;
  }
  *process = _process;
  processes[processSlot] = _process;
out:

  if (ISERR(res)) {
    if (_process && _process->task) {
      taskFree(_process->task);
    }
    // free process
  }
  return res;
}
