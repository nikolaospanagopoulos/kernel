#include "idt.h"
#include "config.h"
#include "io.h"
#include "kernel.h"
#include "memory.h"
idtDesc intDescriptors[osTotalInterupts];
extern void int0h();
extern void int21h();
extern void idt_load(void *ptr);
extern void no_interrupt();
struct idtrDesc idtrDescriptor;

void int21h_handler() {
  print("key pressed");
  outb(0x20, 0x20);
}
void no_interrupt_handler() { outb(0x20, 0x20); }

void int0h_handler() { print("devide by zero error"); }

void idtSet(int interuptNumber, void *address) {

  idtDesc *desc = &intDescriptors[interuptNumber];
  desc->offset_1 = (unsigned long)address & 0x0000ffff;
  desc->selector = codeSelector;
  desc->zero = 0x00;
  desc->typeAttr = 0xEE;
  desc->offset_2 = (unsigned long)address >> 16;
}
void initializeIdt() {
  memset(intDescriptors, 0, sizeof(intDescriptors) / sizeof(intDescriptors[0]));

  idtrDescriptor.limit = sizeof(intDescriptors) - 1;
  idtrDescriptor.base = (unsigned long)intDescriptors;
  for (int i = 0; i < 512; i++) {
    idtSet(i, no_interrupt);
  }

  idtSet(0, int0h);
  idtSet(0x21, int21h);
  idt_load(&idtrDescriptor);
}
