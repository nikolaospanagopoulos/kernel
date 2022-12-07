#include "elf.h"

void *elfGetEntryPtr(struct elfHeader *elfHeader) {
  return (void *)elfHeader->eEntry;
}

uint32_t elfGetEntry(struct elfHeader *elfHeader) { return elfHeader->eEntry; }
