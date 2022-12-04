#include "misc.h"
#include "task.h"

void *isr80hCommand0Sum(struct interruptFrame *frame) {
  int num2 = (int)taskGetTaskItem(taskCurrent(), 1);
  int num1 = (int)taskGetTaskItem(taskCurrent(), 0);
  return (void *)(num1 + num2);
}
