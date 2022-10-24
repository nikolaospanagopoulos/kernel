#include "fat16.h"
#include "disk.h"
#include "kernel.h"
#include "status.h"
#include "string.h"

int fat16Resolve(struct disk *disk);
void *fat16open(struct disk *disk, struct pathPart *path, FILE_MODE mode);

struct fileSystem fat16FileSystem = {

    .resolve = fat16Resolve, .open = fat16open};

struct fileSystem *fat16Init() {
  strcpy(fat16FileSystem.name, "FAT16");

  return &fat16FileSystem;
}
int fat16Resolve(struct disk *disk) { return 0; }

void *fat16open(struct disk *disk, struct pathPart *path, FILE_MODE mode) {
  return 0;
}
