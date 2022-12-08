#include "elfLoader.h"
#include "config.h"
#include "file.h"
#include "kernel.h"
#include "kernelHeap.h"
#include "memory.h"
#include "paging.h"
#include "status.h"
#include "string.h"
#include <stdbool.h>

const char elfSignature[] = {0x7f, 'E', 'L', 'F'};

static bool elfValidSignature(void *buffer) {
  return memcmp(buffer, (void *)elfSignature, sizeof(elfSignature)) == 0;
}

static bool elfValidClass(struct elfHeader *header) {
  // only 32bit

  return header->eIdent[EI_CLASS] == ELFCLASSNONE ||
         header->eIdent[EI_CLASS] == ELFCLASS32;
}

static bool elfValidEncoding(struct elfHeader *header) {
  return header->eIdent[EI_DATA] == ELFDATANONE ||
         header->eIdent[EI_DATA] == ELFDATA2LSB;
}

static bool elfIsExecutable(struct elfHeader *header) {
  return header->eType == ET_EXEC && header->eEntry >= PROGRAM_VIRTUAL_ADDRESS;
}

static bool elfHasProgramHeader(struct elfHeader *header) {
  return header->ePhoff;
}
void *elfMemory(struct elfFile *file) { return file->elfMemory; }

struct elfHeader *elfHeader(struct elfFile *file) {
  return file->elfMemory;
}

struct elf32Shdr *elfSheader(struct elfHeader *header) {
  return (struct elf32Shdr *)((int)header + header->eShoff);
}

struct elf32Phdr *elfPheader(struct elfHeader *header) {
  if (header->ePhoff == 0) {
    return 0;
  }
  return (struct elf32Phdr *)((int)header + header->ePhoff);
}

struct elf32Shdr *elfSection(struct elfHeader *header, int index) {
  return &elfSheader(header)[index];
}

char *elfStrTable(struct elfHeader *header) {
  return (char *)header + elfSection(header, header->eShstrndx)->shOffset;
}
void *elfVirtualBase(struct elfFile *file) { return file->virtualBaseAddress; }
void *elfVirtualEnd(struct elfFile *file) { return file->virtualEndAddress; }
void *elfPhysBase(struct elfFile *file) { return file->physicalBaseAddress; }
void *elfPhysEnd(struct elfFile *file) { return file->physicalEndAddress; }

int elfValidateLoaded(struct elfHeader *header) {
  return (elfValidSignature(header) && elfValidClass(header) &&
          elfValidEncoding(header) && elfHasProgramHeader(header))
             ? ALL_OK
             : -EINFORMAT;
}

int elfProcessPhdrPtLoad(struct elfFile *file, struct elf32Phdr *phdr) {
  if (file->virtualBaseAddress >= (void *)phdr->pVaddr ||
      file->virtualBaseAddress == 0x00) {
    file->virtualBaseAddress = (void *)phdr->pVaddr;
    file->physicalBaseAddress = elfMemory(file) + phdr->pOffset;
  }

  unsigned int endVirtualAddress = phdr->pVaddr + phdr->pFilesz;
  if (file->virtualEndAddress <= (void *)(endVirtualAddress) ||
      file->virtualEndAddress == 0x00) {
    file->virtualEndAddress = (void *)endVirtualAddress;
    file->physicalEndAddress = elfMemory(file) + phdr->pOffset + phdr->pFilesz;
  }
  return 0;
}

void elfClose(struct elfFile *file) {
  if (!file) {
    return;
  }
  kfree(file->elfMemory);
  kfree(file);
}

int elfProcessPheader(struct elfFile *elfFile, struct elf32Phdr *phdr) {
  int res = 0;
  switch (phdr->pType) {
  case PT_LOAD:
    res = elfProcessPhdrPtLoad(elfFile, phdr);
    break;
  }

  return res;
}
struct elf32Phdr *elfProgramHeader(struct elfHeader *header, int index) {
  return &elfPheader(header)[index];
}
int elfProcessHeaders(struct elfFile *elfFile) {
  int res = 0;

  struct elfHeader *header = elfHeader(elfFile);

  for (int i = 0; i < header->ePhnum; i++) {
    struct elf32Phdr *phdr = elfProgramHeader(header, i);
    res = elfProcessPheader(elfFile, phdr);

    if (res < 0) {
      break;
    }
  }

  return res;
}

int elfProcessLoaded(struct elfFile *elfFile) {
  int res = 0;
  struct elfHeader *header = elfHeader(elfFile);
  res = elfValidateLoaded(header);
  if (res < 0) {
    goto out;
  }
  res = elfProcessHeaders(elfFile);
  if (res < 0) {
    goto out;
  }

out:
  return res;
}
int elfLoad(const char *filename, struct elfFile **fileOut) {

  struct elfFile *elfFile = kzalloc(sizeof(struct elfFile));

  int fd = 0;

  int res = fopen(filename, "r");

  if (res <= 0) {
    res = -EIO;
    goto out;
  }

  fd = res;

  struct fileStat stat;
  res = fstat(fd, &stat);

  if (res < 0) {
    goto out;
  }

  elfFile->elfMemory = kzalloc(stat.filesize);
  res = fread(elfFile->elfMemory, stat.filesize, 1, fd);
  if (res < 0) {
    goto out;
    // kfree(elfFile->elfMemory);
  }
  res = elfProcessLoaded(elfFile);

  if (res < 0) {
    goto out;
  }
  *fileOut = elfFile;

out:

  fclose(fd);
  return res;
}
