FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/memory/memory.o ./build/idt/idt.o ./build/io/io.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kernelHeap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/fs/pparser.o ./build/string/string.o ./build/disk/streamer.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/task/tss.asm.o ./build/task/task.o ./build/task/process.o ./build/task/task.asm.o ./build/isr80h/isr80h.o ./build/isr80h/misc.o ./build/isr80h/isr80hIo.o ./build/keyboard/keyboard.o ./build/keyboard/ps2Keyboard.o ./build/formats/elf.o ./build/formats/elfLoader.o ./build/isr80h/isr80heap.o ./build/isr80h/isr80process.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin userprograms
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d
	sudo cp ./hello.txt /mnt/d
	sudo cp ./programs/blank/blank.elf /mnt/d
	sudo cp ./programs/shell/shell.elf /mnt/d
	sudo umount /mnt/d


./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/idt/idt.o: ./src/idt.c
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

./build/fs/file.o: ./src/file.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/file.c -o ./build/fs/file.o
./build/fs/fat/fat16.o: ./src/fat16.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/fat16.c -o ./build/fs/fat/fat16.o
./build/disk/streamer.o: ./src/streamer.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/streamer.c -o ./build/disk/streamer.o

./build/task/task.o: ./src/task.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/task.c -o ./build/task/task.o
./build/task/process.o: ./src/process.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/process.c -o ./build/task/process.o
./build/idt/idt.asm.o: ./src/idt.asm
	nasm -f elf -g ./src/idt.asm -o ./build/idt/idt.asm.o
./build/io/io.asm.o: ./src/io.asm
	nasm -f elf -g ./src/io.asm -o ./build/io/io.asm.o

./build/memory/paging/paging.asm.o: ./src/paging.asm
	nasm -f elf -g ./src/paging.asm -o ./build/memory/paging/paging.asm.o

./build/task/tss.asm.o: ./src/tss.asm
	nasm -f elf -g ./src/tss.asm -o ./build/task/tss.asm.o

./build/task/task.asm.o: ./src/task.asm
	nasm -f elf -g ./src/task.asm -o ./build/task/task.asm.o

./build/gdt/gdt.o: ./src/gdt.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/gdt.c -o ./build/gdt/gdt.o

./build/isr80h/isr80h.o: ./src/isr80h.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/isr80h.c -o ./build/isr80h/isr80h.o
./build/isr80h/misc.o: ./src/misc.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/misc.c -o ./build/isr80h/misc.o
./build/isr80h/isr80hIo.o: ./src/isr80hIo.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/isr80hIo.c -o ./build/isr80h/isr80hIo.o
./build/gdt/gdt.asm.o: ./src/gdt.asm
	nasm -f elf -g ./src/gdt.asm -o ./build/gdt/gdt.asm.o
./build/formats/elf.o: ./src/elf.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/elf.c -o ./build/formats/elf.o
./build/formats/elfLoader.o: ./src/elfLoader.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/elfLoader.c -o ./build/formats/elfLoader.o
./build/keyboard/keyboard.o: ./src/keyboard.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/keyboard.c -o ./build/keyboard/keyboard.o
./build/keyboard/ps2Keyboard.o: ./src/ps2Keyboard.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/ps2Keyboard.c -o ./build/keyboard/ps2Keyboard.o
./build/isr80h/isr80heap.o: ./src/isr80heap.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/isr80heap.c -o ./build/isr80h/isr80heap.o
./build/isr80h/isr80process.o: ./src/isr80process.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/isr80process.c -o ./build/isr80h/isr80process.o
userprograms:
	cd ./programs/stdlib && $(MAKE) all
	cd ./programs/blank && $(MAKE) all
	cd ./programs/shell && $(MAKE) all
userprogramsclean:
	cd ./programs/stdlib && $(MAKE) clean
	cd ./programs/blank && $(MAKE) clean
	cd ./programs/shell && $(MAKE) clean
clean: userprogramsclean
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o
