#pragma once

#include "config.h"
#include "paging.h"
#include "process.h"
#include "stdint.h"

struct registers {

  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  uint32_t ip; // program counter
  uint32_t cs;
  uint32_t flags;
  uint32_t esp;
  uint32_t ss;
};
struct process;
struct task {
  /**
   *page directory of the task
   */
  struct paging4gbChunk *pageDirectory;
  struct process *process;
  struct registers
      registers; // the registers of the task when it is not running

  struct task *next;     // next task
  struct task *previous; // previous task
};

void runFirstEverTask();
int taskPage();
struct task *taskNew(struct process *process);
int taskInit(struct task *task, struct process *process);
int taskFree(struct task *task);
void restoreGeneralPurposeRegisters(struct registers *registers);
void taskReturn(struct registers *registers);
void userRegisters();
int taskSwitch(struct task *task);
