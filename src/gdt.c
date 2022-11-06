#include "gdt.h"
#include "kernel.h"

void encodeGdtEntry(uint8_t *target, struct gdtStructured source) {
  if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
    panic("gdt entry: invalid argument \n");
  }
  target[6] = 0x40;
  if (source.limit > 65536) {
    source.limit = source.limit >> 12;
    target[6] = 0xC0;
  }
  // encodes the limit
  target[0] = source.limit & 0xFF;
  target[1] = (source.limit >> 8) & 0xFF;
  target[6] |= (source.limit >> 16) & 0x0F;

  // encode base
  target[2] = source.base & 0xFF;
  target[3] = (source.base >> 8) & 0xFF;
  target[4] = (source.base >> 16) & 0xFF;
  target[7] = (source.base >> 24) & 0xFF;

  // set type
  target[5] = source.type;
}

void gdtStructuredToGdt(struct gdt *gdt, struct gdtStructured *strGdt,
                        int totalEntries) {
  for (int i = 0; i < totalEntries; i++) {
    encodeGdtEntry((uint8_t *)&gdt[i], strGdt[i]);
  }
}
