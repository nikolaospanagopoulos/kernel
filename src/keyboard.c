#include "keyboard.h"
#include "process.h"
#include "ps2Keyboard.h"
#include "status.h"
#include "task.h"

static struct keyboard *keyboardListHead = 0;
static struct keyboard *keyboardListTail = 0;

void keyboardInit() { keyboardInsert(ps2Init()); }

int keyboardInsert(struct keyboard *keyboard) {
  int res = 0;
  if (keyboard->init == 0) {
    res = -EINVARG;
    goto out;
  }
  if (keyboardListTail) {
    keyboardListTail->next = keyboard;
    keyboardListTail = keyboard;
  } else {
    keyboardListHead = keyboard;
    keyboardListTail = keyboard;
  }
  res = keyboard->init();
out:
  return res;
}

static int keyboardGetTailIndex(struct process *process) {
  return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

void keyboardBackspace(struct process *process) {
  process->keyboard.tail -= 1;
  int realIndex = keyboardGetTailIndex(process);
  process->keyboard.buffer[realIndex] = 0x00;
}

void keyboardPush(char c) {
  struct process *process = processCurrent();

  if (!process) {
    return;
  }
  int realIndex = keyboardGetTailIndex(process);
  process->keyboard.buffer[realIndex] = c;
  process->keyboard.tail++;
}

char keyboardPop() {

  if (!taskCurrent()) {
    return 0;
  }

  struct process *process = taskCurrent()->process;
  int realIndex = process->keyboard.head % sizeof(process->keyboard.buffer);
  char c = process->keyboard.buffer[realIndex];
  if (c == 0x00) {
    return 0;
  }
  process->keyboard.buffer[realIndex] = 0;
  process->keyboard.head++;
  return c;
}
