#include "os.h"
#include "stdio.h"

int osGetKeyBlock() {
  int val = 0;

  do {

    val = osGetKey();

  } while (val == 0);
  return val;
}
void osTerminalReadLine(char *out, int max, bool outputWhileTyping) {
  int i = 0;
  for (i = 0; i < max - 1; i++) {
    char key = osGetKeyBlock();
    if (key == 13) {
      break;
    }

    if (outputWhileTyping) {
      putchar(key);
    }
    if (key == 0x08 && i >= 1) {
      out[i - 1] = 0x00;
      i -= 2;
      continue;
    }
    out[i] = key;
  }
  out[i] = 0x00;
}
