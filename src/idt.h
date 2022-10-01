#pragma once
#include <stdint.h>

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
