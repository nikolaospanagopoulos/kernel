#pragma once
#include <stddef.h>
#include <stdint.h>
#define VGA_WIDTH 80
#define VGA_HEIGHT 20
#define MAX_PATH 108

void print(const char *string);
void terminalInitialize();
void terminalPutChar(int x, int y, char c, char colour);
size_t strlen(const char *str);
uint16_t terminalMakeChar(char c, char colour);
void terminalWriteChar(char c, char colour);
void kernel_main();

#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)
