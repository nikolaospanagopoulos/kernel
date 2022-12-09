#pragma once
#include "config.h"
#include "elfLoader.h"
#include "stdint.h"

#define PROCESS_TYPE_ELF 0
#define PROCESS_TYPE_BINARY 1

typedef unsigned char PROCESS_TYPE;
struct process_allocation {
  void *ptr;
  size_t size;
};

struct commandArgument {
  char argument[512];
  struct commandArgument *next;
};

struct processArguments {
  int argc;
  char **argv;
};

struct process {

  uint16_t id; // process id
  char filename[MAX_PATH];
  // main process task
  struct task *task;
  // the malloced memory of the process
  struct process_allocation allocations[MAX_ALLOCATIONS];
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
  struct processArguments arguments;
};
int processInjectArguments(struct process *process,
                           struct commandArgument *rootArgument);
void processGetArguments(struct process *process, int *argc, char ***argv);
int processLoadForSlot(const char *filename, struct process **process,
                       int processSlot);
int processLoad(const char *filename, struct process **process);
struct process *processCurrent();
int processSwitch(struct process *process);
int processLoadSwitch(const char *filename, struct process **process);
void *processMalloc(struct process *process, size_t size);
void processFree(struct process *process, void *ptr);
