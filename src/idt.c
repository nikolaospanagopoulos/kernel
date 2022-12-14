#include "idt.h"
#include "config.h"
#include "io.h"
#include "kernel.h"
#include "memory.h"
#include "process.h"
#include "status.h"
#include "task.h"
idtDesc intDescriptors[osTotalInterupts];

static ISR80H_COMMAND isr80hCommands[MAX_ISR80_COMMANDS];
extern void *interruptPointerTable[osTotalInterupts];

static INTERRUPT_CALLBACK_FUNCTON interruptCallbacks[osTotalInterupts];

extern void isr80hWrapper();
extern void int0h();
extern void int21h();
extern void idt_load(void *ptr);
extern void no_interrupt();
struct idtrDesc idtrDescriptor;

void no_interrupt_handler() { outb(0x20, 0x20); }

void idtHandleException() {
  processTerminate(taskCurrent()->process);
  taskNext();
}
void idtClock() {

  outb(0x20, 0x20);
  taskNext();
}
void interruptHandler(int interrupt, struct interruptFrame *frame) {

  kernelPage();
  if (interruptCallbacks[interrupt] != 0) {
    taskSaveCurrentState(frame);
    interruptCallbacks[interrupt](frame);
  }
  taskPage();
  outb(0x20, 0x20);
}

void int0h_handler() { print("devide by zero error"); }

int idtRegisterInterruptCallback(int interrupt,
                                 INTERRUPT_CALLBACK_FUNCTON callback) {
  if (interrupt < 0 || interrupt >= osTotalInterupts) {
    return -EINVARG;
  }
  interruptCallbacks[interrupt] = callback;

  return 0;
}

void idtSet(int interuptNumber, void *address) {

  idtDesc *desc = &intDescriptors[interuptNumber];
  desc->offset_1 = (uint32_t)address & 0x0000ffff;
  desc->selector = codeSelector;
  desc->zero = 0x00;
  desc->typeAttr = 0xEE;
  desc->offset_2 = (uint32_t)address >> 16;
}
void initializeIdt() {
  memset(intDescriptors, 0, sizeof(intDescriptors));

  idtrDescriptor.limit = sizeof(intDescriptors) - 1;
  idtrDescriptor.base = (uint32_t)intDescriptors;
  for (int i = 0; i < 512; i++) {
    idtSet(i, interruptPointerTable[i]);
  }

  idtSet(0, int0h);
  idtSet(0x80, isr80hWrapper);

  for (int i = 0; i < 0x20; i++) {
    idtRegisterInterruptCallback(i, idtHandleException);
  }

  idtRegisterInterruptCallback(0x20, idtClock);
  idt_load(&idtrDescriptor);
}

void *isr80hHandleCommand(int command, struct interruptFrame *frame) {

  void *result = 0;

  if (command < 0 || command >= MAX_ISR80_COMMANDS) {
    return 0;
  }
  ISR80H_COMMAND commandFunc = isr80hCommands[command];
  if (!commandFunc) {
    // not an error
    return 0;
  }

  result = commandFunc(frame);
  return result;
}

void isr80RegisterCommand(int commandId, ISR80H_COMMAND command) {
  if (commandId < 0 || commandId >= MAX_ISR80_COMMANDS) {
    panic("command out of bounds");
  }
  if (isr80hCommands[commandId]) {
    panic("you are attempting to overwrite an existing command");
  }
  isr80hCommands[commandId] = command;
}

void *isr80handler(int command, struct interruptFrame *frame) {
  void *res = 0;
  kernelPage();

  taskSaveCurrentState(frame);
  res = isr80hHandleCommand(command, frame);

  taskPage();

  return res;
}
