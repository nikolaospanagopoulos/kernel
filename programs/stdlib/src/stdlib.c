#include "stdlib.h"
#include "os.h"
#include <stdbool.h>

void *malloc(size_t size) { return osMalloc(size); }
void free(void *ptr) { osFree(ptr); }

static void swap(char *x, char *y) {
  char t = *x;
  *x = *y;
  *y = t;
}

static char *reverse(char *buffer, int i, int j) {
  while (i < j) {
    swap(&buffer[i++], &buffer[j--]);
  }
  return buffer;
}

char *itoa(int value, char *buffer, int base) {
  int i = 0;
  bool isNegative = false;

  if (value == 0) {
    buffer[i++] = '0';
    buffer[i] = '\0';
    return buffer;
  }
  if (value < 0 && base == 10) {
    isNegative = true;
    value = -value;
  }

  while (value != 0) {
    int remainder = value % base;
    buffer[i++] = (remainder > 9) ? (remainder - 10) + 65 : remainder + 48;
    value = value / base;
  }

  if (isNegative) {
    buffer[i++] = '-';
  }
  buffer[i] = '\0';
  return reverse(buffer, 0, i - 1);
}
