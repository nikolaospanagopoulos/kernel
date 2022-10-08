#pragma once
#include "config.h"
#include <stddef.h>
#include <stdint.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FREE 0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table {

  HEAP_BLOCK_TABLE_ENTRY *entries;
  size_t total;
};

struct heap {

  struct heap_table *table;
  // start address of the heap data pool
  void *startAddress;
};

int heapCreate(struct heap *heap, void *ptr, void *end,
               struct heap_table *table);
