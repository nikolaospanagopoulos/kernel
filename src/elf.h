#pragma once
#include <stdint.h>

#define PF_X 0x01
#define PF_W 0x02
#define PF_R 0x04
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMYAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 12
#define SHT_HIPROC 13
#define SHT_LOUSER 14
#define SHT_HIUSER 15

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4

#define EI_NIDENT 16
#define EI_CLASS 4
#define EI_DATA 5

#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define SHN_UNDEF 0

typedef uint16_t elf32Half;
typedef uint32_t elf32Word;
typedef int32_t elf32Sword;
typedef uint32_t elf32addr;
typedef int32_t elf32Off;

struct elf32Phdr {

  elf32Word pType;
  elf32Off pOffset;
  elf32addr pVaddr;
  elf32addr pPaddr;
  elf32Word pFilesz;
  elf32Word pMemsz;
  elf32Word pFlags;
  elf32Word pAlign;

} __attribute__((packed));

struct elf32Shdr {

  elf32Word shName;
  elf32Word shType;
  elf32Word shFlags;
  elf32addr shAddr;
  elf32Off shOffset;
  elf32Word shSize;
  elf32Word shLink;
  elf32Word shInfo;
  elf32Word shAddralign;
  elf32Word shEntSize;
} __attribute__((packed));

struct elfHeader {
  unsigned char eIdent[EI_NIDENT];
  elf32Half eType;
  elf32Half eMachine;
  elf32Word eVersion;
  elf32addr eEntry;
  elf32Off ePhoff;
  elf32Off eShoff;
  elf32Word eFlags;
  elf32Half eEhsize;
  elf32Half ePhentsize;
  elf32Half ePhnum;
  elf32Half eShentSize;
  elf32Half eShnum;
  elf32Half eShstrndx;
} __attribute__((packed));

struct elf32Dyn {
  elf32Sword dTag;
  union {
    elf32Word dVal;
    elf32addr dPtr;
  } dUn;
} __attribute__((packed));

struct elf32Sym {
  elf32Word stName;
  elf32addr stValue;
  elf32Word stSize;
  unsigned char stInfo;
  unsigned char stOther;
  elf32Half stShndx;
} __attribute__((packed));

void *elfGetEntryPtr(struct elfHeader *elfHeader);
uint32_t elfGetEntry(struct elfHeader *elfHeader);
