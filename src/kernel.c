#include "kernel.h"
#include "disk.h"
#include "idt.h"
#include "io.h"
#include "kernelHeap.h"
#include "paging.h"

extern void problem();
uint16_t *videomem = 0;
uint16_t terminalRow = 0;
uint16_t terminalCol = 0;
uint16_t terminalMakeChar(char c, char colour) { return (colour << 8) | c; }

void print(const char *string) {

  size_t len = strlen(string);

  for (size_t i = 0; i < len; i++) {

    terminalWriteChar(string[i], 15);
  }
}
struct paging4gbChunk *kernel_chunck = 0;
void kernel_main() {

  terminalInitialize();

  print("hello world!\n");

  kernelHeapInit();
  // disk search and init

  diskSearchAndInit();

  // initialize interupt descriptor table
  initializeIdt();

  // create paging directory
  kernel_chunck = pagingNew4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                               PAGING_ACCESS_FROM_ALL);
  // switch to kernel paging directory
  pagingSwitch(get4GbchunckDirectory(kernel_chunck));

  // enable paging
  enable_paging();

  // enable interrupts
  enable_interrupts();
}

void terminalPutChar(int x, int y, char c, char colour) {

  videomem[(y * VGA_WIDTH) + x] = terminalMakeChar(c, colour);
}

void terminalWriteChar(char c, char colour) {

  if (c == '\n') {
    terminalRow += 1;
    terminalCol = 0;
    return;
  }
  terminalPutChar(terminalCol, terminalRow, c, colour);
  terminalCol += 1;

  if (terminalCol >= VGA_WIDTH) {
    terminalCol = 0;
    terminalRow += 1;
  }
}

size_t strlen(const char *str) {

  size_t len = 0;
  while (str[len]) {

    len++;
  }

  return len;
}

void terminalInitialize() {
  videomem = (uint16_t *)(0xB8000);

  terminalRow = 0;
  terminalCol = 0;
  for (int y = 0; y < VGA_HEIGHT; y++) {
    for (int x = 0; x < VGA_WIDTH; x++) {
      terminalPutChar(x, y, ' ', 0);
    }
  }
}
