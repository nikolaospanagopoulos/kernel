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
  int res = 0;
  if (index < 0 || index >= MAX_PROCESSES) {
    res = -EINVARG;
  }
out:
  if (ISERR(res)) {
    return ERROR(res);
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
int processMapMemory(struct process *process) {
  int res = 0;
  res = processMapBinary(process);
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
  res = processMapMemory(process);
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
