#include "fat16.h"
#include "config.h"
#include "disk.h"
#include "file.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "pparser.h"
#include "status.h"
#include "streamer.h"
#include "string.h"
#include <stdint.h>

#define EXTENDED_BOOT_SIGNATURE 0x29
#define FAT16_BAD_SECTOR 0xFF7
#define FAT_ENTRY_SIZE 0x02
#define FAT_UNUSED 0x00

typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

// bitmasks for fat directory entry
#define FAT_FILE_READ_ONLY 0b00000001
#define FAT_FILE_HIDDEN 0b00000010
#define FAT_FILE_SYSTEM                                                        \
  0b00000100 // System. Indicates that the file belongs to the system and must
             // not be physically moved
#define FAT_FILE_VOLUME_LABEL 0b00001000
#define FAT_FILE_SUBDIRECTORY 0b00010000
#define FAT_FILE_ARCHIVED 0b00100000
#define FAT_FILE_DEVICE 0b01000000
#define FAT_FILE_RESERVED 0b10000000

struct fat_header_extended_bios_parameter_block {

  uint8_t driveNumber;
  uint8_t windowsNtBit;
  uint8_t signature;
  uint32_t volumeId;
  uint8_t volumeIdString[11];
  uint8_t systemIdString[8];
} __attribute__((packed));

struct fat_header_bios_parameter_block {

  uint8_t shortJumpInstruction[3]; // the machine code of the jump instruction
                                   // will load in this array
  uint8_t oemiIdentifier[8];
  uint16_t BytesPerSector;
  uint8_t SectorPerCluster;
  uint16_t ReservedSectors;
  uint8_t fatCopies;
  uint16_t rootDirectoryEntries;
  uint16_t numberOfSectors;
  uint8_t mediaType;
  uint16_t sectorsPerFat;
  uint16_t sectorsPerTrack;
  uint16_t numberOfHeads;
  uint32_t HiddenSectors;
  uint32_t sectorsBig;

} __attribute__((packed));

struct fatH {
  struct fat_header_bios_parameter_block primaryHeader;
  union fatHeaderExt {

    struct fat_header_extended_bios_parameter_block extendedHeader;
  } shared;
};

// directory item
struct fatDirectoryItem {
  uint8_t filename[8];
  uint8_t ext[3];
  uint8_t attribute;
  uint8_t reserved;
  uint8_t creationTimeTenthsOfSec;
  uint16_t creationTime;
  uint16_t creationDate;
  uint16_t lastAccess;
  uint16_t
      high16bitsFirstCluster; // if subdirectory , high 16 of where the ather
                              // directory items are stored, if is the file, it
                              // points to where the first data of the file is;
  uint16_t lastModificationTime;
  uint16_t lastModificationDate;
  uint16_t low16bitsFirstCluster;
  uint32_t filesize;
} __attribute__((packed));

struct fatDirectory {
  struct fatDirectoryItem *item; // array of items
  int total;
  int sectorPosition;       // first sector where fat directory is
  int endingSectorPosition; // last sector where fat directory is
};

struct fatItem { // either directory or file
  union {
    struct fatDirectoryItem *item;  // if file
    struct fatDirectory *directory; // if directory
  };
  FAT_ITEM_TYPE type;
};

struct fatFileDescriptor { // open file
  struct fatItem *item;
  uint32_t position;
};

struct fatPrivateInfo {
  struct fatH header;
  struct fatDirectory rootDirectory;
  // used to stream data clusters
  struct diskStream *clusterReadStream;
  // used to stream the file Allocation table
  struct diskStream *fatReadStream;
  // when we stream the directory
  struct diskStream *directoryStream;
};

int fat16Resolve(struct disk *disk);
void *fat16open(struct disk *disk, struct pathPart *path, FILE_MODE mode);

struct fileSystem fat16FileSystem = {

    .resolve = fat16Resolve, .open = fat16open};

struct fileSystem *fat16Init() {
  strcpy(fat16FileSystem.name, "FAT16");

  return &fat16FileSystem;
}

static void fat16InitPrivate(struct disk *disk,
                             struct fatPrivateInfo *privateData) {
  memset(privateData, 0, sizeof(struct fatPrivateInfo));
  privateData->clusterReadStream = diskstreamerNew(disk->id);
  privateData->fatReadStream = diskstreamerNew(disk->id);
  privateData->directoryStream = diskstreamerNew(disk->id);
}
int fat16SectorToAbsolute(struct disk *disk, int sector) {
  return sector * disk->sectorSize;
}
int fat16GetTotalItemsForDirectory(struct disk *disk, uint32_t dirStartSector) {
  struct fatDirectoryItem item;
  struct fatDirectoryItem emptyItem;

  memset(&emptyItem, 0, sizeof(emptyItem));

  struct fatPrivateInfo *fatPrivate = disk->fsPrivate;
  int res = 0;
  int i = 0;
  int directoryStartPosition = dirStartSector * disk->sectorSize;
  struct diskStream *stream = fatPrivate->directoryStream;
  if (diskStreamerSeek(stream, directoryStartPosition) != ALL_OK) {
    res = -EIO;
    goto out;
  }
  while (1) {
    if (diskstreamerRead(stream, &item, sizeof(item)) != ALL_OK) {
      res = -EIO;
      goto out;
    }
    if (item.filename[0] == 0x00) {
      break;
    }
    if (item.filename[0] == 0xE5) {
      continue; // unused item;
    }
    i++;
  }
  res = i;
out:
  return res;
}
int fat16GetRootDirectory(struct disk *disk, struct fatPrivateInfo *fatPrivate,
                          struct fatDirectory *directory) {
  int res = 0;

  struct fat_header_bios_parameter_block *primaryHeader =
      &fatPrivate->header.primaryHeader;
  // root directory position
  int rootDirectoryPosition =
      (primaryHeader->fatCopies * primaryHeader->sectorsPerFat) +
      primaryHeader->ReservedSectors;
  int rootDirectoryEntries = primaryHeader->rootDirectoryEntries;
  int rootDirSize = rootDirectoryEntries * sizeof(struct fatDirectoryItem);
  int totalSectors = rootDirSize / disk->sectorSize;
  if (rootDirSize % disk->sectorSize) {
    // read one more sector if size is greater than the sectorSize;
    totalSectors += 1;
  }

  int totalItems = fat16GetTotalItemsForDirectory(disk, rootDirectoryPosition);
  struct fatDirectoryItem *dir = kzalloc(rootDirSize);
  if (!dir) {
    res = -ENOMEM;
    goto out;
  }

  struct diskStream *stream = fatPrivate->directoryStream;
  if (diskStreamerSeek(stream, fat16SectorToAbsolute(
                                   disk, rootDirectoryPosition)) != ALL_OK) {
    res = -EIO;
    goto out;
  }

  if (diskstreamerRead(stream, dir, rootDirSize) != ALL_OK) {
    res = -EIO;
    goto out;
  }

  directory->item = dir;
  directory->total = totalItems;
  directory->sectorPosition = rootDirectoryPosition;
  directory->endingSectorPosition =
      rootDirectoryPosition + (rootDirSize / disk->sectorSize);

out:
  return res;
}
int fat16Resolve(struct disk *disk) {
  int res = 0;
  struct fatPrivateInfo *fatPrivate = kzalloc(sizeof(struct fatPrivateInfo));
  fat16InitPrivate(disk, fatPrivate);

  disk->fsPrivate = fatPrivate;
  disk->fileSystem = &fat16FileSystem;
  struct diskStream *stream = diskstreamerNew(disk->id);

  if (!stream) {
    res = -ENOMEM;
    goto out;
  }
  if (diskstreamerRead(stream, &fatPrivate->header,
                       sizeof(fatPrivate->header)) != ALL_OK) {
    res = -EIO;
    goto out;
  }

  // if there is no 0x29 signature, it is not FAT16
  if (fatPrivate->header.shared.extendedHeader.signature != 0x29) {
    // we return -> not out fileSystem
    res = -EFSNOTUS;
    goto out;
  }

  if (fat16GetRootDirectory(disk, fatPrivate, &fatPrivate->rootDirectory) !=
      ALL_OK) {
    res = -EIO;
    goto out;
  }

out:
  if (stream) {
    diskStreamerFree(stream);
  }
  if (res < 0) {
    kfree(fatPrivate);
    disk->fsPrivate = 0;
  }
  return res;
}
// null terminated instead of space
void fat16ToProper(char **out, const char *in) {
  while (*in != 0x00 && *in != 0x20) {
    **out = *in;
    *out += 1;
    in += 1;
  }
  if (*in == 0x20) {
    **out = 0x00;
  }
}

void fat16GetFullRelativeFilename(struct fatDirectoryItem *item, char *out,
                                  int maxLen) {
  memset(out, 0x00, maxLen);
  char *outTmp = out;
  fat16ToProper(&outTmp, (const char *)item->filename);
  if (item->ext[0] != 0x00 && item->ext[0] != 0x20) {
    *outTmp++ = '.';
    fat16ToProper(&outTmp, (const char *)item->ext);
  }
}

static uint32_t fat16GetFirstFatSector(struct fatPrivateInfo *private) {
  // end of reserved sectors is where first fat allocation table is
  return private->header.primaryHeader.ReservedSectors;
}

static int fat16GetFatEntry(struct disk *disk, int cluster) {
  int res = -1;
  struct fatPrivateInfo *fatPrivate = disk->fsPrivate;
  struct diskStream *stream = fatPrivate->fatReadStream;
  if (!stream) {
    goto out;
  }
  uint32_t fatTablePosition =
      fat16GetFirstFatSector(fatPrivate) * disk->sectorSize;

  res = diskStreamerSeek(stream, fatTablePosition * (cluster * FAT_ENTRY_SIZE));
  if (res < 0) {
    goto out;
  }
  uint32_t result;
  res = diskstreamerRead(stream, &result, sizeof(result));
  if (!res) {
    goto out;
  }
  res = result;
out:
  return res;
}

// get cluster based on starting cluster and offset
static int fat16GetClusterForOffset(struct disk *disk, int startingCluster,
                                    int offset) {
  int res = 0;
  struct fatPrivateInfo *private = disk->fsPrivate;

  int sizeOfClusterInBytes =
      private->header.primaryHeader.SectorPerCluster * disk->sectorSize;
  int clusterToUse = startingCluster;
  int clustersAhead = offset / sizeOfClusterInBytes;
  for (int i = 0; i < clustersAhead; i++) {
    int entry = fat16GetFatEntry(disk, clusterToUse);
    if (entry == 0xFF8 || entry == 0xFFF) {
      // we are at the last entry
      res = -EIO;
      goto out;
    }
    // check if is bad sector
    if (entry == FAT16_BAD_SECTOR) {
      res = -EIO;
      goto out;
    }
    // reserved sector
    if (entry == 0xFF0 || entry == 0xFF6) {
      res = -EIO;
      goto out;
    }
    // there was no cluster
    if (entry == 0x00) {
      res = -EIO;
      goto out;
    }
    clusterToUse = entry;
  }
  res = clusterToUse;
out:
  return res;
}

static int fat16ClusterToSector(struct fatPrivateInfo *private, int cluster) {
  return private->rootDirectory.endingSectorPosition +
         ((cluster - 2) * private->header.primaryHeader.SectorPerCluster);
}
static int fat16ReadInternalFromStream(struct disk *disk,
                                       struct diskStream *stream, int cluster,
                                       int offset, int total, void *out) {
  int res = 0;

  struct fatPrivateInfo *private = disk->fsPrivate;
  int sizeOfClusterInBytes =
      private->header.primaryHeader.SectorPerCluster * disk->sectorSize;

  int clusterToUse = fat16GetClusterForOffset(disk, cluster, offset);

  if (clusterToUse < 0) {
    res = clusterToUse;
    goto out;
  }

  int offsetFromCluster = offset % sizeOfClusterInBytes;
  int startingSector = fat16ClusterToSector(private, clusterToUse);
  int startingPosition =
      (startingSector * disk->sectorSize) * offsetFromCluster;
  int totalToRead = total > sizeOfClusterInBytes ? sizeOfClusterInBytes : total;
  res = diskStreamerSeek(stream, startingPosition);
  if (res != ALL_OK) {
    goto out;
  }
  res = diskstreamerRead(stream, out, totalToRead);
  if (res != ALL_OK) {
    goto out;
  }
  total -= totalToRead;
  if (total > 0) {
    // we have more to read
    res = fat16ReadInternalFromStream(
        disk, stream, cluster, offset + totalToRead, total, out + totalToRead);
  }

out:
  return res;
}
static int fat16readInternal(struct disk *disk, int startingCluster, int offset,
                             int total, void *out) {
  struct fatPrivateInfo *private = disk->fsPrivate;
  struct diskStream *stream = private->clusterReadStream;
  return fat16ReadInternalFromStream(disk, stream, startingCluster, offset,
                                     total, out);
}
static uint32_t fat16getFirstCluster(struct fatDirectoryItem *item) {
  // join high and low bits

  return item->high16bitsFirstCluster | item->low16bitsFirstCluster;
}
void fat16FreeDirectory(struct fatDirectory *directory) {
  if (!directory) {
    return;
  }
  if (directory->item) {
    kfree(directory->item);
  }
  kfree(directory);
}
void fat16FatItemFree(struct fatItem *item) {
  if (item->type == FAT_ITEM_TYPE_DIRECTORY) {
    fat16FreeDirectory(item->directory);
  } else if (item->type == FAT_ITEM_TYPE_FILE) {
    kfree(item->item);
  }
  kfree(item);
}
struct fatDirectory *fat16LoadFatDirectory(struct disk *disk,
                                           struct fatDirectoryItem *item) {
  int res = 0;
  struct fatDirectory *directory = 0;
  struct fatPrivateInfo *fatPrivate = disk->fsPrivate;

  if (!(item->attribute & FAT_FILE_SUBDIRECTORY)) {
    res = -EINVARG;
    goto out;
  }
  directory = kzalloc(sizeof(struct fatDirectory));
  if (!directory) {
    res = -ENOMEM;
    goto out;
  }
  int cluster = fat16getFirstCluster(item);
  int clusterSector = fat16ClusterToSector(fatPrivate, cluster);
  int totalItems = fat16GetTotalItemsForDirectory(disk, clusterSector);
  directory->total = totalItems;
  int directorySize = directory->total * sizeof(struct fatDirectoryItem);
  directory->item = kzalloc(directorySize);
  if (!directory->item) {
    res = -ENOMEM;
    goto out;
  }
  res = fat16readInternal(disk, cluster, 0x00, directorySize, directory->item);
  if (res != ALL_OK) {
    goto out;
  }
out:
  if (res != ALL_OK) {
    fat16FreeDirectory(directory);
  }
  return directory;
}

struct fatDirectoryItem *fat16CloneDirectoryItem(struct fatDirectoryItem *item,
                                                 int size) {
  struct fatDirectoryItem *copiedItem = 0;

  if (size < sizeof(struct fatDirectoryItem)) {
    return 0;
  }
  copiedItem = kzalloc(size);
  if (!copiedItem) {
    return 0;
  }

  memcpy(copiedItem, item, size);

  return copiedItem;
}

struct fatItem *fat16NewFatItemForDirectoryItem(struct disk *disk,
                                                struct fatDirectoryItem *item) {
  struct fatItem *fatItem = kzalloc(sizeof(struct fatItem));
  if (!fatItem) {
    return NULL;
  }

  if (item->attribute & FAT_FILE_SUBDIRECTORY) {
    // check if it is subdirectory and not file
    fatItem->directory = fat16LoadFatDirectory(disk, item);
    fatItem->type = FAT_ITEM_TYPE_DIRECTORY;
  }
  fatItem->type = FAT_ITEM_TYPE_FILE;
  fatItem->item =
      fat16CloneDirectoryItem(item, sizeof(struct fatDirectoryItem));
  return fatItem;
}
struct fatItem *fat16FindItemInDirectory(struct disk *disk,
                                         struct fatDirectory *directory,
                                         const char *name) {
  struct fatItem *fatItem = 0;
  char tmpFilename[MAX_PATH];
  for (int i = 0; i < directory->total; i++) {
    fat16GetFullRelativeFilename(&directory->item[i], tmpFilename,
                                 sizeof(tmpFilename));
    if (istrncmp(tmpFilename, name, sizeof(tmpFilename)) == 0) {
      // all is good
      fatItem = fat16NewFatItemForDirectoryItem(disk, &directory->item[0]);
    }
  }
  return fatItem;
}
struct fatItem *fat16GetDirectoryEntry(struct disk *disk,
                                       struct pathPart *path) {
  struct fatPrivateInfo *fatPrivate = disk->fsPrivate;
  struct fatItem *currentItem = 0;

  struct fatItem *rootItem =
      fat16FindItemInDirectory(disk, &fatPrivate->rootDirectory, path->part);
  if (!rootItem) {
    goto out;
  }

  struct pathPart *nextPart = path->next;
  currentItem = rootItem;
  while (nextPart != 0) {
    if (currentItem->type != FAT_ITEM_TYPE_DIRECTORY) {
      currentItem = 0;
      break;
    }
    struct fatItem *tmpItem =
        fat16FindItemInDirectory(disk, currentItem->directory, nextPart->part);
    fat16FatItemFree(currentItem);
    currentItem = tmpItem;
    nextPart = nextPart->next;
  }
out:
  return currentItem;
}
void *fat16open(struct disk *disk, struct pathPart *path, FILE_MODE mode) {
  if (mode != FILE_MODE_READ) {
    return ERROR(-ERDONLY);
  }
  struct fatFileDescriptor *descriptor = 0;
  descriptor = kzalloc(sizeof(struct fatFileDescriptor));
  if (!descriptor) {
    return ERROR(-ENOMEM);
  }
  descriptor->item = fat16GetDirectoryEntry(disk, path);

  if (!descriptor->item) {
    return ERROR(-EIO);
  }

  descriptor->position = 0;
  return descriptor;
}
