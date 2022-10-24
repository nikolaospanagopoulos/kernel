#include "file.h"
#include "config.h"
#include "fat16.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "status.h"

struct fileSystem *fileSystems[MAX_FILESYSTEMS];
struct fileDescriptor *fileDescriptors[MAX_FILE_DESCRIPTORS];

static struct fileSystem **fsGetFreeFileSystem() {
  for (int i = 0; i < MAX_FILESYSTEMS; i++) {
    if (fileSystems[i] == 0) {

      return &fileSystems[i];
    }
  }
  return 0;
}
// for filesystems to be able to insert themselves
void fsInsertFilesystem(struct fileSystem *fileSystem) {
  struct fileSystem **fs;

  fs = fsGetFreeFileSystem();
  if (!fs) {
    print("problem inserting fileSystem");
    while (1) {
    };
  }

  *fs = fileSystem;
}

// load filesystems built into the kernel
static void fsStaticLoad() { fsInsertFilesystem(fat16Init()); }

void fsLoad() {

  memset(fileSystems, 0, sizeof(fileSystems));
  fsStaticLoad();
}
void fsInit() {
  memset(fileDescriptors, 0, sizeof(fileDescriptors));
  fsLoad();
}

static int fileNewDescriptor(struct fileDescriptor **descriptorOut) {
  int res = -ENOMEM;

  for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
    struct fileDescriptor *fileDescriptorNew =
        kzalloc(sizeof(struct fileDescriptor));
    // descriptors start at index 1
    fileDescriptorNew->index = i + 1;
    fileDescriptors[i] = fileDescriptorNew;
    *descriptorOut = fileDescriptorNew;
    res = 0;
    break;
  }
  return res;
}

static struct fileDescriptor *fileGetDescriptor(int fileDescriptorId) {
  if (fileDescriptorId <= 0 || fileDescriptorId >= MAX_FILE_DESCRIPTORS) {
    return 0;
  }

  int index = fileDescriptorId - 1;

  return fileDescriptors[index];
}
struct fileSystem *fsResolve(struct disk *disk) {
  struct fileSystem *fs = 0;

  for (int i = 0; i < MAX_FILESYSTEMS; i++) {
    if (fileSystems[i] != 0 && fileSystems[i]->resolve(disk) == 0) {
      fs = fileSystems[i];
      break;
    }
  }
  return fs;
}

int fopen(const char *filename, const char *mode) { return -EIO; }
