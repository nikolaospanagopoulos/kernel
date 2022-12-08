FILES= ./build/shell.o  
INCLUDES= ../stdlib/src
FLAGS= -g

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ./shell.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../stdlib/stdlib.elf
./build/shell.o: ./src/shell.c
	i686-elf-gcc -I${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/shell.c -o ./build/shell.o
clean:
	rm -rf ${FILES}
	rm ./shell.elf
