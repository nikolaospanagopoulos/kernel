#include "string.h"

size_t strlen(const char *ptr) {
  size_t i = 0;
  while (*ptr != 0) {
    i++;
    ptr += 1;
  }

  return i;
}

size_t strnlen(const char *ptr, size_t max) {
  size_t i = 0;

  for (i = 0; i < max; i++) {
    if (ptr[i] == 0) {
      break;
    }
  }

  return i;
}
bool isDigit(char c) { return c >= 48 && c <= 57; }

int toNumericDigit(char c) { return c - 48; }

char *strcpy(char *dest, const char *src) {
  char *tmp = dest;

  while (*src != 0) {
    *dest = *src;
    src += 1;
    dest += 1;
  };

  *dest = 0x00;

  return tmp;
}
