#include "heap.h"
#include "config.h"
#include "memory.h"
#include "status.h"
#include <stddef.h>

static int validateHeapTable(void *ptr, void *end, struct heap_table *table) {
  int res = 0;

  size_t tableSize = (size_t)(end - ptr);
  size_t totalBlocks = tableSize / heapBlockSize;

  if (table->total != totalBlocks) {
    res = -EINVARG;
    goto out;
  }
out:
  return res;
}

static int validateHeapAllignment(void *ptr) {
  return ((unsigned int)ptr % heapBlockSize) == 0;
}

int heapCreate(struct heap *heap, void *ptr, void *end,
               struct heap_table *table) {

  int res = 0;

  // make sure the memory taken follows the 4096 bytes rule. every pointer
  // should point to 4096 bytes
  if (!validateHeapAllignment(ptr) || !validateHeapAllignment(end)) {
    res = -EINVARG;
    goto out;
  }

  memset(heap, 0, sizeof(struct heap));
  heap->startAddress = ptr;
  heap->table = table;

  res = validateHeapTable(ptr, end, table);
  if (res < 0) {
    goto out;
  }

  size_t tableSize = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
  memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, tableSize);
out:
  return res;
}

static uint32_t allignHeapValueToUpper(uint32_t val) {
  if ((val % heapBlockSize) == 0) {
    return val;
  }

  val = (val - (val % heapBlockSize));
  val += heapBlockSize;

  return val;
}

void *heapMalloc(struct heap *heap, size_t size) {

  size_t allignedSize = allignHeapValueToUpper(size);
  uint32_t total_blocks = allignedSize / heapBlockSize;
  return heapMallocBlocks(heap, total_blocks);
}

void *heapMallocBlocks(struct heap *heap, uint32_t totalBlocksToAllocate) {

  void *address = 0;
  // checks if it can find the needed contigouus blocks. if it does, returns the
  // first one
  int start_block = startBlock(heap, totalBlocksToAllocate);

  if (start_block < 0) {
    goto out;
  }

  address = heapBlockAddress(heap, start_block);

  // mark the blocks as taken so no other program can take them
  heapBlocksMarkAsTaken(heap, start_block, totalBlocksToAllocate);

out:
  return address;
}
static uint32_t getEntryType(uint32_t val) { return val & 0x0f; }
int startBlock(struct heap *heap, uint32_t blocksToAllocate) {

  struct heap_table *heap_table = heap->table;
  // the first block that we find free
  int bs = -1;
  // the current block
  int bc = 0;

  for (size_t i = 0; i < heap_table->total; i++) {
    if (getEntryType(heap_table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
      bc = 0;
      bs = -1;
      continue;
    }
    if (bs == -1) {
      bs = i;
    }
    bc++;
    if (bc == blocksToAllocate) {

      break;
    }
  }
  if (bs == -1) {
    return -ENOMEM;
  }
  return bs;
}
void *heapBlockAddress(struct heap *heap, int start_block) {

  return heap->startAddress + (start_block * heapBlockSize);
}
void heapBlocksMarkAsTaken(struct heap *heap, int start_block,
                           int total_blocks) {
  int end_block = (start_block + total_blocks) - 1;

  HEAP_BLOCK_TABLE_ENTRY entry =
      HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;

  if (total_blocks > 1) {
    entry |= HEAP_BLOCK_HAS_NEXT;
  }

  for (int i = start_block; i <= end_block; i++) {
    heap->table->entries[i] = entry;
    entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
    // check later
    if (i != end_block) {
      entry |= HEAP_BLOCK_HAS_NEXT;
    }
  }
}
void heapFree(struct heap *heap, void *ptr) {

  heapMarkBlocksFree(heap, heapAddressToBlock(heap, ptr));
}
int heapAddressToBlock(struct heap *heap, void *address) {
  return ((int)(address - heap->startAddress)) / heapBlockSize;
}

void heapMarkBlocksFree(struct heap *heap, size_t startBlock) {
  struct heap_table *heapTable = heap->table;
  for (size_t i = startBlock; i < heapTable->total; i++) {
    HEAP_BLOCK_TABLE_ENTRY entry = heapTable->entries[i];
    heapTable->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
    if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
      break;
    }
  }
}
