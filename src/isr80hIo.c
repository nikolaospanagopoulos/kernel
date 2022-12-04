#include "isr80hIo.h"
#include "kernel.h"
#include "task.h"

void *isr80hCommand1Print(struct interruptFrame *frame) {

  void *userMessageBuffer =
      taskGetTaskItem(taskCurrent(), 0); // get first item pushed to stack
  char buff[1024];
  copyStringFromTask(taskCurrent(), userMessageBuffer, buff, sizeof(buff));

  print(buff);

  return 0;
}
