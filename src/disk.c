#include "disk.h"
#include "config.h"
#include "io.h"
#include "memory.h"
#include "status.h"
#include <stddef.h>

struct disk disk;
int diskReadSector(int lba, int total, void *buf) {
  // we need to output data to these ports
  outb(0x1F6, (lba >> 24) | 0xE0);
  outb(0x1F2, total);
  outb(0x1F3, (unsigned char)(lba & 0xff));
  outb(0x1F4, (unsigned char)(lba >> 8));
  outb(0x1F5, (unsigned char)(lba >> 16));
  outb(0x1F7, 0x20);

  // we are going to read to bytes at the time. so cast to short
  unsigned short *ptr = (unsigned short *)buf;

  for (size_t b = 0; b < total; b++) {
    // wait for the buffer to be ready
    char c = insb(0x1F7);

    while (!(c & 0x08)) {
      c = insb(0x1F7);
    }
    // copy from hard disk to memory
    for (int i = 0; i < 256; i++) {
      *ptr = insw(0x1F0);
      ptr++;
    }
  }

  return 0;
}
void diskSearchAndInit() {
  memset(&disk, 0, sizeof(disk));
  disk.type = DISK_TYPE_REAL;
  disk.sectorSize = SECTOR_SIZE;
}

struct disk *getDisk(int index) {
  if (index != 0) {
    return 0;
  }

  return &disk;
}

int diskReadBlock(struct disk *idisk, unsigned int lba, int total, void *buff) {
  if (idisk != &disk) {
    return -EIO;
  }
  return diskReadSector(lba, total, buff);
}
