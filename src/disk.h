#pragma once
#include "file.h"
// real physical hard disk

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_REAL 0

struct disk {
  DISK_TYPE type;
  int sectorSize;
  struct fileSystem *fileSystem;
  int id;

  // private data of fileSystem
  void *fsPrivate;
};

void diskSearchAndInit();
struct disk *getDisk(int index);
int diskReadBlock(struct disk *idisk, unsigned int lba, int total, void *buff);
