#include "paging.h"
#include "heap.h"
#include "kernelHeap.h"
#include "status.h"
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
    directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITABLE;
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

int pagingGetIndexes(void *virtualAddress, uint32_t *directoryIndexOut,
                     uint32_t *tableIndexOut) {

  int res = 0;

  if (!pagingIsAlligned(virtualAddress)) {
    res = -EINVARG;
    goto out;
  }
  *directoryIndexOut = ((uint32_t)virtualAddress /
                        (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));

  *tableIndexOut =
      ((uint32_t)virtualAddress %
       (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);

out:
  return res;
}

bool pagingIsAlligned(void *address) {

  return ((uint32_t)address % PAGING_PAGE_SIZE) == 0;
}
int pagingSet(uint32_t *directory, void *virt, uint32_t val) {

  if (!pagingIsAlligned(virt)) {

    return -EINVARG;
  }

  uint32_t directoryIndex = 0;
  uint32_t tableIndex = 0;

  int res = pagingGetIndexes(virt, &directoryIndex, &tableIndex);
  if (res < 0) {
    return res;
  }

  uint32_t entry = directory[directoryIndex];

  // extract table address so we can access it directly
  uint32_t *table = (uint32_t *)(entry & 0xfffff000);
  table[tableIndex] = val;

  return 0;
}
