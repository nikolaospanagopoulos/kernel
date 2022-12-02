#pragma once
#include <stdint.h>

struct interruptFrame;

typedef void *(*ISR80H_COMMAND)(struct interruptFrame *frame);

typedef struct idtDesc {
  uint16_t offset_1;
  uint16_t selector;
  uint8_t zero;
  uint8_t typeAttr;
  uint16_t offset_2;

} __attribute__((packed)) idtDesc;

struct idtrDesc {

  uint16_t limit;
  uint32_t base;

} __attribute__((packed));

void idtSet(int interuptNumber, void *address);
void initializeIdt();
void enable_interrupts();
void disable_interrupts();

struct interruptFrame {

  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t reserved;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t ip;
  uint32_t cs;
  uint32_t flags;
  uint32_t esp;
  uint32_t ss;

} __attribute__((packed));

void isr80RegisterCommand(int commandId, ISR80H_COMMAND command);
