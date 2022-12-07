#pragma once

#include "config.h"
#include "elf.h"
#include <stddef.h>
#include <stdint.h>

struct elfFile {

  char filename[MAX_PATH];

  int inMemorySize;

  // the physical memory address that the elf file is loaded at
  void *elfMemory;
  // virtual base address of the binary
  void *virtualBaseAddress;
  void *virtualEndAddress;
  void *physicalBaseAddress;
  void *physicalEndAddress;
};

int elfLoad(const char *filename, struct elfFile **fileOut);
void elfClose(struct elfFile *file);
void *elfVirtualBase(struct elfFile *file);
void *elfVirtualEnd(struct elfFile *file);
void *elfPhysBase(struct elfFile *file);
void *elfPhysEnd(struct elfFile *file);
struct elfHeader *elfHeader(struct elfFile *file);
struct elf32Phdr *elfPheader(struct elfHeader *header);
void *elfMemory(struct elfFile *file);
