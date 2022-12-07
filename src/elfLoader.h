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
