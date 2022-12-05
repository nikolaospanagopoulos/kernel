#include "isr80hIo.h"
#include "kernel.h"
#include "keyboard.h"
#include "task.h"

void *isr80hCommand1Print(struct interruptFrame *frame) {

  void *userMessageBuffer =
      taskGetTaskItem(taskCurrent(), 0); // get first item pushed to stack
  char buff[1024];
  copyStringFromTask(taskCurrent(), userMessageBuffer, buff, sizeof(buff));

  print(buff);

  return 0;
}

void *isr80hCommand2GetKey(struct interruptFrame *frame) {

  char c = keyboardPop();
  return (void *)((int)c);
}

void *isr80hCommand3PutChar(struct interruptFrame *frame) {
  char c = (char)(int)taskGetTaskItem(taskCurrent(), 0);
  terminalWriteChar(c, 15);
  return 0;
}
