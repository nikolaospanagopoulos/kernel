[BITS 32]

section .asm

global _start

_start:

loop:
	call getKey
	;push message
	push eax
    mov eax, 3
	int 0x80
	add esp,4
	jmp loop

getKey:
    mov eax, 2
	int 0x80
	cmp eax, 0x00
	je getKey
	ret

section .data
message: db 'Hello!', 0
