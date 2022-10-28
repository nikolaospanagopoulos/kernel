#pragma once
#include <stdbool.h>
#include <stddef.h>

int strncmp(const char *str, const char *str2, int numOfBytes);
size_t strlen(const char *ptr);
bool isDigit(char c);
int toNumericDigit(char c);
size_t strnlen(const char *ptr, size_t max);
char *strcpy(char *dest, const char *src);
char toLower(char s1);
int strnlenTerminator(const char *str, int max, char terminator);
int istrncmp(const char *s1, const char *s2, int numOfBytes);
