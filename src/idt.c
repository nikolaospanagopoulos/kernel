#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory.h"
idtDesc intDescriptors[osTotalInterupts];

extern void idt_load(void *ptr);

struct idtrDesc idtrDescriptor;

void idtZero() { print("devide by zero error"); }

void idtSet(int interuptNumber, void *address) {

  idtDesc *desc = &intDescriptors[interuptNumber];
  desc->offset_1 = (unsigned long)address & 0x0000ffff;
  desc->selector = codeSelector;
  desc->zero = 0x00;
  desc->typeAttr = 0xEE;
  desc->offset_2 = (unsigned long)address >> 16;
}
void initializeIdt() {
  memset(intDescriptors, 0, 512);

  idtrDescriptor.limit = 512 - 1;
  idtrDescriptor.base = (unsigned long)intDescriptors;

  idtSet(0, idtZero);

  idt_load(&idtrDescriptor);
}
