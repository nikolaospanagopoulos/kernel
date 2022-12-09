#pragma once
#include <stdbool.h>
#include <stddef.h>

struct commandArgument {
  char argument[512];
  struct commandArgument *next;
};
struct processArguments {
  int argc;
  char **argv;
};
void print(const char *message);
int osGetKey();
void *osMalloc(size_t size);
void osFree(void *ptr);
void osPutchar(char c);
int osGetKeyBlock();
void osTerminalReadLine(char *out, int max, bool outputWhileTyping);
void osProcessLoadStart(const char *filename);
struct commandArgument *osParseCommand(const char *command, int max);
void osProcessGetArguments(struct processArguments *arguments);
