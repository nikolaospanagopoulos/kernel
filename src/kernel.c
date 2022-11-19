#include "kernel.h"
#include "config.h"
#include "disk.h"
#include "file.h"
#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "kernelHeap.h"
#include "memory.h"
#include "paging.h"
#include "pparser.h"
#include "streamer.h"
#include "tss.h"

extern void problem();
uint16_t *videomem = 0;
uint16_t terminalRow = 0;
uint16_t terminalCol = 0;
uint16_t terminalMakeChar(char c, char colour) { return (colour << 8) | c; }

void panic(const char *msg) {
  print(msg);
  while (1) {
  }
}

void print(const char *string) {

  size_t len = strlen(string);

  for (size_t i = 0; i < len; i++) {

    terminalWriteChar(string[i], 15);
  }
}
struct paging4gbChunk *kernel_chunck = 0;
struct tss tss;
struct gdt gdtReal[TOTAL_GDT_SEGMENTS];
struct gdtStructured gdtStructured[TOTAL_GDT_SEGMENTS] = {

    // NULL SEGMENT
    {.base = 0x00, .limit = 0x00, .type = 0x00},
    // kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},
    // kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},
    // user code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},
    // user data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},
    // tss
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9}

};

void kernel_main() {

  terminalInitialize();

  memset(gdtReal, 0x00, sizeof(gdtReal));
  gdtStructuredToGdt(gdtReal, gdtStructured, TOTAL_GDT_SEGMENTS);
  // load gdt
  gdtLoad(gdtReal, sizeof(gdtReal));

  kernelHeapInit();

  // initialize file system
  fsInit();

  // disk search and init

  diskSearchAndInit();

  // initialize interupt descriptor table
  initializeIdt();

  // setup tss
  memset(&tss, 0x00, sizeof(tss));
  tss.esp0 = 0x600000;
  tss.ss0 = dataSelector;
  tssLoad(0x28);

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
    struct fileStat stat;
    fstat(fd, &stat);
    fclose(fd);
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
