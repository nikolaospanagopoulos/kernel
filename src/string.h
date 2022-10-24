#pragma once
#include <stdbool.h>
#include <stddef.h>

size_t strlen(const char *ptr);
bool isDigit(char c);
int toNumericDigit(char c);
size_t strnlen(const char *ptr, size_t max);
char *strcpy(char *dest, const char *src);
