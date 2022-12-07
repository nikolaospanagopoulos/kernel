#pragma once
#include "config.h"
#include "elfLoader.h"
#include "stdint.h"

#define PROCESS_TYPE_ELF 0
#define PROCESS_TYPE_BINARY 1

typedef unsigned char PROCESS_TYPE;

struct process {

  uint16_t id; // process id
  char filename[MAX_PATH];
  // main process task
  struct task *task;
  // the malloced memory of the process
  void *allocations[MAX_ALLOCATIONS];
  PROCESS_TYPE filetype;
  union {
    // the physical memory of the process
    void *ptr;
    struct elfFile *elfFile;
  };
  // the pointer to the stack memory
  void *stack;
  // the size of the data pointed to by ptr
  uint32_t size;

  struct keyboardBuffer {
    char buffer[KEYBOARD_BUFFER_SIZE];
    int tail;
    int head;
  } keyboard;
};
int processLoadForSlot(const char *filename, struct process **process,
                       int processSlot);
int processLoad(const char *filename, struct process **process);
struct process *processCurrent();
int processSwitch(struct process *process);
int processLoadSwitch(const char *filename, struct process **process);
void *processMalloc(struct process *process, size_t size);
