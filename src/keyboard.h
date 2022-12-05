#pragma once
#include "process.h"

struct process;
typedef int (*keyboardInitFunction)();

struct keyboard {
  keyboardInitFunction init;
  char name[20];
  struct keyboard *next;
};
void keyboardInit();
void keyboardBackspace(struct process *process);
void keyboardPush(char c);
char keyboardPop();
int keyboardInsert(struct keyboard *keyboard);
