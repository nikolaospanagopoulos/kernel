#include "kernel.h"
#include "disk.h"
#include "file.h"
#include "idt.h"
#include "io.h"
#include "kernelHeap.h"
#include "paging.h"
#include "pparser.h"
#include "streamer.h"

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

  kernelHeapInit();

  // initialize file system
  fsInit();

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

  int fd = fopen("0:/hello.txt", "r");
  if (fd) {
    print("\nwe opened hello\n");
    char buff[13];

    fread(buff, 13, 1, fd);
    print(buff);
  }
  while (1) {
  }
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
