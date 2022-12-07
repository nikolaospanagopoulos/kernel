#include "isr80heap.h"
#include "task.h"
#include <stddef.h>

void *isr80hCommand4Malloc(struct interruptFrame *frame) {
  size_t size = (int)taskGetTaskItem(taskCurrent(), 0);
  return processMalloc(taskCurrent()->process, size);

  return 0;
}
