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
int strncmp(const char *str, const char *str2, int numOfBytes) {
  unsigned char u1, u2;

  while (numOfBytes > 0) {

    u1 = (unsigned char)*str++;
    u2 = (unsigned char)*str2++;
    if (u1 != u2) {
      return u1 - u2;
    }
    if (u1 == '\0') {
      return 0;
    }

    numOfBytes--;
  }
  return 0;
}

int strnlenTerminator(const char *str, int max, char terminator) {
  int i = 0;
  for (i = 0; i < max; i++) {
    if (str[i] == '\0' || str[i] == terminator) {
      break;
    }
  }
  return i;
}

int istrncmp(const char *s1, const char *s2, int numOfBytes) {
  unsigned char u1, u2;

  while (numOfBytes > 0) {

    u1 = (unsigned char)*s1++;
    u2 = (unsigned char)*s2++;
    if (u1 != u2 && toLower(u1) != toLower(u2)) {
      return u1 - u2;
    }
    if (u1 == '\0') {
      return 0;
    }

    numOfBytes--;
  }
  return 0;
};

char toLower(char s1) {
  if (s1 >= 65 && s1 <= 90) {
    s1 += 32;
  }
  return s1;
}

char *strncpy(char *dest, const char *src, int count) {
  int i = 0;
  for (i = 0; i < count - 1; i++) {
    if (src[i] == 0x00) {
      break;
    }
    dest[i] = src[i];
  }
  dest[i] = 0x00;
  return dest;
}
