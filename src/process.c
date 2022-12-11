#include "process.h"
#include "config.h"
#include "elfLoader.h"
#include "file.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "status.h"
#include "string.h"
#include "task.h"
#include <stdbool.h>

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

int processSwitch(struct process *process) {
  currentProcess = process;

  return 0;
}

int processTerminateAllocations(struct process *process) {
  for (int i = 0; i < MAX_ALLOCATIONS; i++) {
    processFree(process, process->allocations[i].ptr);

    // maybe use Kfree() for speed???????
  }
  return 0;
}

int processFreeBinaryData(struct process *process) {
  kfree(process->ptr);
  return 0;
}

int processFreeElfData(struct process *process) {
  elfClose(process->elfFile);
  return 0;
}

int processFreeProgramData(struct process *process) {
  int res = 0;
  switch (process->filetype) {
  case PROCESS_TYPE_BINARY:
    res = processFreeBinaryData(process);
    break;
  case PROCESS_TYPE_ELF:
    res = processFreeElfData(process);
    break;
  default:
    res = -EINVARG;
  }

  return res;
}
static void processSwitchToAny() {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i]) {
      processSwitch(processes[i]);
      return;
    }
  }
  panic("no process to switch to!!");
}

static void processUnlink(struct process *process) {
  processes[process->id] = 0x00;
  if (currentProcess == process) {
    processSwitchToAny();
  }
}
int processTerminate(struct process *process) {
  int res = 0;
  res = processTerminateAllocations(process);
  if (res < 0) {
    goto out;
  }
  res = processFreeProgramData(process);
  if (res < 0) {
    goto out;
  }
  // free stack memory
  kfree(process->stack);
  taskFree(process->task);
  processUnlink(process);

  print("process was teminated");
out:
  return res;
}

static bool processIsProcessPtr(struct process *process, void *ptr) {
  for (int i = 0; i < MAX_ALLOCATIONS; i++) {
    if (process->allocations[i].ptr == ptr) {
      return true;
    }
  }
  return false;
}

static void processAllocationsUnjoin(struct process *process, void *ptr) {
  for (int i = 0; i < MAX_ALLOCATIONS; i++) {
    if (process->allocations[i].ptr == ptr) {
      process->allocations[i].ptr = 0x00;
      process->allocations[i].size = 0;
    }
  }
}

static struct process_allocation *processGetAllocation(struct process *process,
                                                       void *addr) {

  for (int i = 0; i < MAX_ALLOCATIONS; i++) {
    if (process->allocations[i].ptr == addr) {
      return &process->allocations[i];
    }
  }
  return 0;
}
void processFree(struct process *process, void *ptr) {
  struct process_allocation *allocation = processGetAllocation(process, ptr);
  if (!allocation) {
    return;
  }
  int res = pagingMapTo(process->task->pageDirectory, allocation->ptr,
                        allocation->ptr,
                        pagingAlignAddress(allocation->ptr + allocation->size),
                        PAGING_IS_PRESENT);
  if (res < 0) {
    return;
  }
  processAllocationsUnjoin(process, ptr);
  kfree(ptr);
}

static int findFreeAllocationIndex(struct process *process) {
  int res = -ENOMEM;

  for (int i = 0; i < MAX_ALLOCATIONS; i++) {
    if (process->allocations[i].ptr == 0) {
      res = i;
      break;
    }
  }
  return res;
}
void *processMalloc(struct process *process, size_t size) {

  void *ptr = kzalloc(size);
  if (!ptr) {
    return 0;
  }
  int index = findFreeAllocationIndex(process);

  if (index < 0) {
    goto out_err;
  }
  int res = pagingMapTo(
      process->task->pageDirectory, ptr, ptr, pagingAlignAddress(ptr + size),
      PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
  if (res < 0) {
    goto out_err;
  }
  process->allocations[index].ptr = ptr;
  process->allocations[index].size = size;
  return ptr;
out_err:
  if (ptr) {
    kfree(ptr);
  }
  return 0;
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
  process->filetype = PROCESS_TYPE_BINARY;
  process->ptr = programDataPtr;
  process->size = stat.filesize;
out:
  fclose(fd);
  return res;
}
static int processLoadElf(const char *filename, struct process *process) {
  int res = 0;
  struct elfFile *elfFile = 0;
  res = elfLoad(filename, &elfFile);
  if (res < 0) {
    goto out;
  }
  process->filetype = PROCESS_TYPE_ELF;
  process->elfFile = elfFile;
out:
  return res;
}

static int processLoadData(const char *filename, struct process *process) {
  int res = 0;

  res = processLoadElf(filename, process);

  if (res == -EINFORMAT) {
    res = processLoadBinary(filename, process);
  }
  return res;
}
int processMapBinary(struct process *process) {
  int res = 0;

  pagingMapTo(process->task->pageDirectory, (void *)PROGRAM_VIRTUAL_ADDRESS,
              process->ptr, pagingAlignAddress(process->ptr + process->size),
              PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);

  return res;
}
void *elfPhdrPhysAddress(struct elfFile *file, struct elf32Phdr *phdr) {
  return elfMemory(file) + phdr->pOffset;
}
static int processMapElf(struct process *process) {
  int res = 0;

  struct elfFile *elfFile = process->elfFile;
  struct elfHeader *header = elfHeader(elfFile);
  struct elf32Phdr *pheaders = elfPheader(header);

  for (int i = 0; i < header->ePhnum; i++) {
    struct elf32Phdr *phdr = &pheaders[i];
    void *phdrPhysAddress = elfPhdrPhysAddress(elfFile, phdr);
    int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
    if (phdr->pFlags & PF_W) {
      flags |= PAGING_IS_WRITABLE;
    }
    res =
        pagingMapTo(process->task->pageDirectory,
                    pagingAlignToLowerPage((void *)phdr->pVaddr),
                    pagingAlignToLowerPage(phdrPhysAddress),
                    pagingAlignAddress(phdrPhysAddress + phdr->pMemsz), flags);
    if (ISERR(res)) {
      break;
    }
  }

  return res;
}

int processMapMemory(struct process *process) {
  int res = 0;

  switch (process->filetype) {

  case PROCESS_TYPE_ELF:
    res = processMapElf(process);
    break;
  case PROCESS_TYPE_BINARY:
    res = processMapBinary(process);
    break;
  default:
    panic("processMapMemory: invalid file type\n");
  }
  if (res < 0) {
    goto out;
  }
  pagingMapTo(process->task->pageDirectory,
              (void *)PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack,
              pagingAlignAddress(process->stack + PROGRAM_STACK_SIZE),
              PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);
out:
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

int processLoadSwitch(const char *filename, struct process **process) {
  int res = processLoad(filename, process);

  if (res == ALL_OK) {
    processSwitch(*process);
  }
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
    goto out;
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
void processGetArguments(struct process *process, int *argc, char ***argv) {
  *argc = process->arguments.argc;
  *argv = process->arguments.argv;
}

int processCountCommandArguments(struct commandArgument *rootArgument) {
  struct commandArgument *current = rootArgument;
  int i = 0;
  while (current) {
    current = current->next;
    i++;
  }
  return i;
}

int processInjectArguments(struct process *process,
                           struct commandArgument *rootArgument) {

  int res = 0;

  struct commandArgument *current = rootArgument;

  int i = 0;

  int argc = processCountCommandArguments(rootArgument);

  if (argc == 0) {
    res = -EIO;
    goto out;
  }

  char **argv = processMalloc(process, sizeof(const char *) * argc);
  if (!argv) {
    res = -ENOMEM;
    goto out;
  }
  while (current) {
    char *argumentString = processMalloc(process, sizeof(current->argument));
    if (!argumentString) {
      res = -EIO;
      goto out;
    }
    strncpy(argumentString, current->argument, sizeof(current->argument));
    argv[i] = argumentString;
    current = current->next;
    i++;
  }
  process->arguments.argc = argc;
  process->arguments.argv = argv;
out:
  return res;
}
void *isr80hCommand9Exit(struct interruptFrame *frame) {
  struct process *process = taskCurrent()->process;
  processTerminate(process);
  taskNext();
  return 0;
}
