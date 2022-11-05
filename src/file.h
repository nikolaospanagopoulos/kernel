#pragma once
#include "disk.h"
#include "pparser.h"
#include <stdint.h>

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

typedef int (*FS_READ_FUNCTION)(struct disk *disk, void *privateData,
                                uint32_t size, uint32_t numMemBlocks,
                                char *out);
typedef int (*FS_SEEK_FUNCTION)(void *privateData, uint32_t offset,
                                FILE_SEEK_MODE mode);

typedef int (*FS_CLOSE_FUNCTION)(void *privateData);

enum STAT_MODE { // set if read only
  FILE_STAT_READ_ONLY = 0b00000001
};
typedef enum STAT_MODE FILE_STAT_FLAGS;
struct fileStat {
  FILE_STAT_FLAGS flags;
  uint32_t filesize;
};
typedef int (*FS_STAT_FUNCTION)(struct disk *disk, void *privateData,
                                struct fileStat *stat);

struct fileSystem {
  // resolve function should return 0 if the disk is using a fileSystem the
  // kernel can manage
  FS_RESOLVE_FUNCTION resolve;
  FS_OPEN_FUNCTION open;
  FS_READ_FUNCTION read;
  FS_SEEK_FUNCTION seek;
  FS_STAT_FUNCTION stat;
  FS_CLOSE_FUNCTION close;
  // file system name ex FAT16, FAT32 , NTFS etc
  char name[20];
};

struct fileDescriptor {
  int index;
  struct fileSystem *fileSystem;
  void *privateData;
  struct disk *disk;
};

void fsInit();
int fopen(const char *filename, const char *modeStr);
void fsInsertFilesystem(struct fileSystem *fileSystem);
struct fileSystem *fsResolve(struct disk *disk);
int fread(void *ptr, uint32_t size, uint32_t numMemBlocks, int fd);
int fseek(int fd, int offset, FILE_SEEK_MODE mode);
int fstat(int fd, struct fileStat *stat);
int fclose(int fd);
