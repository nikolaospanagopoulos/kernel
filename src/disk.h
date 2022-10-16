#pragma once
// real physical hard disk

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_REAL 0

struct disk {
  DISK_TYPE type;
  int sectorSize;
};

void diskSearchAndInit();
struct disk *getDisk(int index);
int diskReadBlock(struct disk *idisk, unsigned int lba, int total, void *buff);
