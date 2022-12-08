#pragma once
#include <stdbool.h>
#include <stddef.h>
void print(const char *message);
int osGetKey();
void *osMalloc(size_t size);
void osFree(void *ptr);
void osPutchar(char c);
int osGetKeyBlock();
void osTerminalReadLine(char *out, int max, bool outputWhileTyping);
void osProcessLoadStart(const char *filename);
