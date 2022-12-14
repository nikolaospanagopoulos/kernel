#pragma once
#define PAGING_CACHE_DISABLED 0b00010000
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001
#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct paging4gbChunk {
  uint32_t *directoryEntry;
};

struct paging4gbChunk *pagingNew4gb(uint8_t flags);
void pagingSwitch(struct paging4gbChunk *directory);
uint32_t *get4GbchunckDirectory(struct paging4gbChunk *chunk);
void enable_paging();
int pagingGetIndexes(void *virtualAddress, uint32_t *directoryIndexOut,
                     uint32_t *tableIndexOut);
bool pagingIsAlligned(void *address);
int pagingSet(uint32_t *directory, void *virt, uint32_t val);
void pagingFree4gb(struct paging4gbChunk *chunk);
int pagingMapTo(struct paging4gbChunk *directory, void *virt, void *physical,
                void *psysEnd, int flags);
int pagingMapRange(struct paging4gbChunk *directory, void *virt, void *phys,
                   int count, int flags);
int pagingMap(struct paging4gbChunk *directory, void *virt, void *phys,
              int flags);
void *pagingAlignAddress(void *ptr);
uint32_t pagingGet(uint32_t *directory, void *virt);
void *pagingAlignToLowerPage(void *addr);
