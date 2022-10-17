#include "memory.h"

void *memset(void *ptr, int c, size_t size) {

  char *charPtr = (char *)ptr;
  for (size_t i = 0; i < size; i++) {
    charPtr[i] = (char)c;
  }

  return ptr;
}

int memcmp(void *ptr1, void *ptr2, int count) {
  unsigned char *p = ptr1;
  unsigned char *p2 = ptr2;
  int status = 0;

  // they point to same memory
  if (ptr1 == ptr2) {
    return status;
  }

  while (count > 0) {
    if (*p != *p2) {
      status = *p > *p2 ? 1 : -1;
      break;
    }

    count--;
    p++;
    p2++;
  }
  return status;
}
