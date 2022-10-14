#include "paging.h"
#include "heap.h"
#include "kernelHeap.h"
#include <stdint.h>

void paging_load_directory(uint32_t *directory);
static uint32_t *currentDirectory = 0;
struct paging4gbChunk *pagingNew4gb(uint8_t flags) {

  uint32_t *directory =
      kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);

  int offset = 0;
  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {

    uint32_t *entry =
        kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
      entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
    }

    offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
    directory[i] = (unsigned long)entry | flags | PAGING_IS_WRITABLE;
  }
  struct paging4gbChunk *chunk4Gb = kzalloc(sizeof(struct paging4gbChunk));
  chunk4Gb->directoryEntry = directory;
  return chunk4Gb;
}

uint32_t *get4GbchunckDirectory(struct paging4gbChunk *chunk) {
  return chunk->directoryEntry;
}

void pagingSwitch(uint32_t *directory) {
  paging_load_directory(directory);
  currentDirectory = directory;
}
