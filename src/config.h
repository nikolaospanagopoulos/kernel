#pragma once

#define heapSize 104857600
#define heapBlockSize 4096
#define osTotalInterupts 512
#define codeSelector 0x08
#define dataSelector 0x10
#define heapAddress 0x01000000
#define heapTableAddress 0x00007E00
#define SECTOR_SIZE 512
#define MAX_PATH 108
#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512
#define TOTAL_GDT_SEGMENTS 6
#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_VIRTUAL_STACK_ADDRESS_END                                      \
  PROGRAM_VIRTUAL_STACK_ADDRESS_START - PROGRAM_STACK_SIZE
#define MAX_ALLOCATIONS 1024
#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b
#define MAX_PROCESSES 12
#define MAX_ISR80_COMMANDS 1024
