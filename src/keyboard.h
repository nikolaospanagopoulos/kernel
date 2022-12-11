#pragma once
#include "process.h"

#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0

struct process;
typedef int (*keyboardInitFunction)();

struct keyboard {
  keyboardInitFunction init;
  char name[20];
  struct keyboard *next;
  int capsLockState;
};
void keyboardInit();
void keyboardBackspace(struct process *process);
void keyboardPush(char c);
char keyboardPop();
int keyboardInsert(struct keyboard *keyboard);
int keyboardGetCapsLockState(struct keyboard *keyboard);
void keyboardSetCapsLock(struct keyboard *keyboard, int state);
