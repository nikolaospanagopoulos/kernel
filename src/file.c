#include "file.h"
#include "config.h"
#include "disk.h"
#include "fat16.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "pparser.h"
#include "status.h"
#include "string.h"

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

FILE_MODE fetFileModeByString(const char *str) {
  FILE_MODE mode = FILE_MODE_INVALID;
  if (strncmp(str, "r", 1) == 0) {
    mode = FILE_MODE_READ;
  }
  if (strncmp(str, "w", 1) == 0) {
    mode = FILE_MODE_WRITE;
  }
  if (strncmp(str, "a", 1) == 0) {
    mode = FILE_MODE_APPEND;
  }

  return mode;
}

int fopen(const char *filename, const char *modeStr) {
  int res = 0;

  struct pathRoot *rootPath = parse(filename, NULL);
  if (!rootPath) {
    res = -EINVARG;
    goto out;
  }
  // we can not have just root path. we need filename
  if (!rootPath->first) {
    res = -EINVARG;
    goto out;
  }
  // in case we cannot read from specific drive
  struct disk *disk = getDisk(rootPath->driveNumber);
  if (!disk) {
    res = -EIO;
    goto out;
  }

  if (!disk->fileSystem) {
    res = -EIO;
    goto out;
  }
  FILE_MODE mode = fetFileModeByString(modeStr);
  if (mode == FILE_MODE_INVALID) {
    res = -EINVARG;
    goto out;
  }
  void *descriptorPrivateData =
      disk->fileSystem->open(disk, rootPath->first, mode);
  if (ISERR(descriptorPrivateData)) {
    res = ERROR_I(descriptorPrivateData);
    goto out;
  }

  struct fileDescriptor *desc = 0;
  res = fileNewDescriptor(&desc);

  if (res < 0) {
    goto out;
  }
  desc->fileSystem = disk->fileSystem;
  desc->privateData = descriptorPrivateData;
  desc->disk = disk;
  res = desc->index;
out:
  if (res < 0) {
    res = 0;
  }
  return res;
}
int fread(void *ptr, uint32_t size, uint32_t numMemBlocks, int fd) {
  int res = 0;

  if (size == 0 || numMemBlocks == 0 || fd < 1) {
    res = -EINVARG;
    goto out;
  }
  struct fileDescriptor *desc = fileGetDescriptor(fd);
  if (!desc) {
    res = -EINVARG;
    goto out;
  }

  res = desc->fileSystem->read(desc->disk, desc->privateData, size,
                               numMemBlocks, (char *)ptr);

out:
  return res;
}

int fseek(int fd, int offset, FILE_SEEK_MODE mode) {
  int res = 0;

  struct fileDescriptor *desc = fileGetDescriptor(fd);
  if (!desc) {
    res = -EIO;
    goto out;
  }
  res = desc->fileSystem->seek(desc->privateData, offset, mode);

out:
  return res;
}
