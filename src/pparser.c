#include "pparser.h"
#include "kernel.h"
#include "memory.h"
#include "string.h"

static int pathIsValid(const char *filename) {
  int length = strnlen(filename, MAX_PATH);
  return (length >= 3 && isDigit(filename[0]) &&
          memcmp((void *)&filename[1], ":/", 2) == 0);
}
