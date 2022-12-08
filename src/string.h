#pragma once
#include <stdbool.h>
#include <stddef.h>

int strncmp(const char *str, const char *str2, int numofbytes);
size_t strlen(const char *ptr);
bool isdigit(char c);
int toNumericDigit(char c);
size_t strnlen(const char *ptr, size_t max);
char *strcpy(char *dest, const char *src);
char toLower(char s1);
bool isDigit(char c);
int strnlenterminator(const char *str, int max, char terminator);
int istrncmp(const char *s1, const char *s2, int numofbytes);
char *strncpy(char *dest, const char *src, int count);
