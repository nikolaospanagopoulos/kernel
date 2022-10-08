#include "kernelHeap.h"
#include "config.h"
#include "heap.h"
#include "kernel.h"

struct heap kernelHeap;
struct heap_table kernelHeapTable;

void kernelHeapInit() {

  int totalTableEntries = heapSize / heapBlockSize;
  kernelHeapTable.entries = (HEAP_BLOCK_TABLE_ENTRY *)heapTableAddress;
  kernelHeapTable.total = totalTableEntries;
  void *end = (void *)heapSize + heapAddress;

  int res = heapCreate(&kernelHeap, (void *)heapAddress, end, &kernelHeapTable);

  if (res < 0) {
    print("error in heap creation");
  }
}
