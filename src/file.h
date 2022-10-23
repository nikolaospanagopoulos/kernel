#pragma once
#include "disk.h"
#include "pparser.h"

enum SEEK_MODE { SEEK_SET, SEEK_CUR, SEEK_END };

typedef enum SEEK_MODE FILE_SEEK_MODE;

enum MODE {
  FILE_MODE_READ,
  FILE_MODE_WRITE,
  FILE_MODE_APPEND,
  FILE_MODE_INVALID
};
typedef enum MODE FILE_MODE;

struct disk;

typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct pathPart *path,
                                  FILE_MODE mode);

typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

struct fileSystem {
  // resolve function should return 0 if the disk is using a fileSystem the
  // kernel can manage
  FS_RESOLVE_FUNCTION resolve;
  FS_OPEN_FUNCTION open;
  // file system name ex FAT16, FAT32 , NTFS etc
  char name[20];
};

struct fileDescriptor {
  int index;
  struct fileSystem *fileSystem;
  void *privateData;
  struct disk *disk;
};

void fs_init();
int fopen(const char *filename, const char *mode);
void fsInsertFilesystem(struct fileSystem *fileSystem);
struct fileSystem *fsResolve(struct disk *disk);
