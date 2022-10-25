#include "fat16.h"
#include "streamer.h"
#include "disk.h"
#include "kernel.h"
#include "status.h"
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

//directory item
struct fatDirectoryItem{
	uint8_t filename[8];
	uint8_t ext[3];
	uint8_t attribute;
	uint8_t reserved;
	uint8_t creationTimeTenthsOfSec;
	uint16_t creationTime;
	uint16_t creationDate;
	uint16_t lastAccess;
	uint16_t high16bitsFirstCluster; //if subdirectory , high 16 of where the ather directory items are stored, if is the file, it points to where the first data of the file is;
	uint16_t lastModificationTime;
	uint16_t lastModificationDate;
	uint16_t low16bitsFirstCluster;
	uint32_t filesize;
}__attribute__((packed));


struct fatDirectory{
	struct fatDirectoryItem*item; //array of items
	int total;
	int sectorPosition; // first sector where fat directory is
	int endingSectorPosition; //last sector where fat directory is
};


struct fatItem{ //either directory or file
	union{
		struct fatDirectoryItem*item;//if file
		struct fatDirectory*directory;//if directory
	};
	FAT_ITEM_TYPE type;
};

struct fatItemDescriptor{ //open file
	struct fatItem *item;
	uint32_t position;
};

struct fatPrivateInfo{
	struct fatH header;
	struct fatDirectory rootDirectory;
	//used to stream data clusters
	struct stream *clusterReadStream;
	//used to stream the file Allocation table
	struct stream *fatReadStream;
	//when we stream the directory
	struct stream*directoryStream;
};


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
