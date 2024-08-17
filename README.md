# Custom 32-bit Kernel

A 32-bit kernel with features such as a heap memory manager, custom filesystem, and multitasking capabilities.

## Features

- **Paging:** Implements memory paging for efficient memory management.
- **Heap Memory Manager:** Custom memory allocator for dynamic memory management.
- **Multitasking:** Supports multiple processes running simultaneously.
- **FAT16 File System:** Basic file system implementation to handle storage operations.

## Requirements

To build and run this kernel on your computer, ensure you have the following installed:

- **Linux Operating System**
- **QEMU** - Emulator for testing
- **GDB** - GNU Debugger for debugging
- **NASM** - Netwide Assembler for assembling x86 assembly code
- **i686-elf-gcc** - Cross-compiler targeting the i686 architecture
- **Make** - Build automation tool

## Setup Instructions

Follow these steps to set up and run the kernel on your system:

1. **Clone the Repository:**

   ```bash
   git clone <repository_url>
   sudo mkdir -p /mnt/d
   make
   qemu-system-x86_64 -hda ./bin/os.bin
   ```
