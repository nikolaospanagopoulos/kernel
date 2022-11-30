#pragma once

#include <stdint.h>
struct gdt {

  uint16_t segment;
  uint16_t baseFirst;
  uint8_t base;
  uint8_t access;
  uint8_t highFlags;
  uint8_t base24_31Flags;
} __attribute__((packed));

struct gdtStructured {
  uint32_t base;
  uint32_t limit;
  uint8_t type;
};

void gdtLoad(struct gdt *gdt, int size);
void gdtStructuredToGdt(struct gdt *gdt, struct gdtStructured *strGdt,
                        int totalEntries);
