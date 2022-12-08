#include "isr80heap.h"
#include "task.h"
#include <stddef.h>

void *isr80hCommand4Malloc(struct interruptFrame *frame) {
  size_t size = (int)taskGetTaskItem(taskCurrent(), 0);
  return processMalloc(taskCurrent()->process, size);
}
void *isr80hCommand5Free(struct interruptFrame *frame) {
  void *ptrToFree = taskGetTaskItem(taskCurrent(), 0);
  processFree(taskCurrent()->process, ptrToFree);
  return 0;
}
