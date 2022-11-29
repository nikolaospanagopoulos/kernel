#include "paging.h"
#include "heap.h"
#include "kernelHeap.h"
#include "status.h"
#include <stdint.h>

void *pagingAlignAddress(void *ptr) {
  if ((uint32_t)ptr % PAGING_PAGE_SIZE) {
    return (void *)((uint32_t)ptr + PAGING_PAGE_SIZE -
                    ((uint32_t)ptr % PAGING_PAGE_SIZE));
  }

  return ptr;
}

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

void pagingFree4gb(struct paging4gbChunk *chunk) {
  for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
    uint32_t entry = chunk->directoryEntry[i];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    kfree(table);
  }
  kfree(chunk->directoryEntry);
  kfree(chunk);
}

int pagingMap(uint32_t *directory, void *virt, void *phys, int flags) {
  if (((unsigned int)virt % PAGING_PAGE_SIZE) ||
      ((unsigned int)phys % PAGING_PAGE_SIZE)) {
    return -EINVARG;
  }
  return pagingSet(directory, virt, (uint32_t)phys | flags);
}

int pagingMapRange(uint32_t *directory, void *virt, void *phys, int count,
                   int flags) {
  int res = 0;
  for (int i = 0; i < count; i++) {

    res = pagingMap(directory, virt, phys, flags);
    if (res == 0) {
      break;
      virt += PAGING_PAGE_SIZE;
      phys += PAGING_PAGE_SIZE;
    }
  }
  return res;
}
int pagingMapTo(uint32_t *directory, void *virt, void *physical, void *physEnd,
                int flags) {
  int res = 0;

  if ((uint32_t)virt % PAGING_PAGE_SIZE) {
    res = -EINVARG;
    goto out;
  }

  if ((uint32_t)physical % PAGING_PAGE_SIZE) {
    res = -EINVARG;
    goto out;
  }
  if ((uint32_t)physEnd % PAGING_PAGE_SIZE) {
    res = -EINVARG;
    goto out;
  }
  if ((uint32_t)physEnd < (uint32_t)physical) {
    res = -EINVARG;
    goto out;
  }

  uint32_t totalBytes = physEnd - physical;

  int totalPages = totalBytes / PAGING_PAGE_SIZE;

  res = pagingMapRange(directory, virt, physical, totalPages, flags);

out:

  return res;
}
