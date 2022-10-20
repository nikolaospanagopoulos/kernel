FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/memory/memory.o ./build/idt/idt.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kernelHeap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/fs/pparser.o ./build/string/string.o ./build/disk/streamer.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/idt/idt.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./src/memory.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/memory.c -o ./build/memory/memory.o
./build/memory/heap/heap.o: ./src/heap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/heap.c -o ./build/memory/heap/heap.o
./build/memory/heap/kernelHeap.o: ./src/kernelHeap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernelHeap.c -o ./build/memory/heap/kernelHeap.o

./build/memory/paging/paging.o: ./src/paging.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/paging.c -o ./build/memory/paging/paging.o

./build/disk/disk.o: ./src/disk.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/disk.c -o ./build/disk/disk.o
./build/string/string.o: ./src/string.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/string.c -o ./build/string/string.o
./build/fs/pparser.o: ./src/pparser.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/pparser.c -o ./build/fs/pparser.o
./build/disk/streamer.o: ./src/streamer.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/streamer.c -o ./build/disk/streamer.o
./build/idt/idt.asm.o: ./src/idt.asm
	nasm -f elf -g ./src/idt.asm -o ./build/idt/idt.asm.o
./build/io/io.asm.o: ./src/io.asm
	nasm -f elf -g ./src/io.asm -o ./build/io/io.asm.o

./build/memory/paging/paging.asm.o: ./src/paging.asm
	nasm -f elf -g ./src/paging.asm -o ./build/memory/paging/paging.asm.o

clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o
