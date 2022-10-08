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
  return ((unsigned long)ptr % heapBlockSize) == 0;
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
