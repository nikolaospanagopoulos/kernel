ORG 0
BITS 16

__start:
   jmp short start
   nop

times 33 db 0

start:
    jmp 0x7c0:step2


step2:

   cli
   mov ax, 0x7c0
   mov ds, ax
   mov es, ax
   mov ax, 0x00
   mov ss, ax
   mov sp, 0x7c00
   sti
   mov ah, 2
   mov al, 1
   mov ch, 0
   mov cl, 2
   mov dh, 0
   mov bx, buffer
   int 0x13

   jc error


   mov si, buffer
   call print
   jmp $


error:
    mov si, error
	call print
   jmp $
print:
   mov bx, 0
.loop:
   lodsb ;print the character the si is pointing to
   cmp al, 0
   je .done
   call print_char
   jmp .loop
.done:
   ret
print_char:

   mov ah, 0eh
   int 0x10
   ret
error_message: db 'Failed to load sector' ,0


times 510- ($ - $$ ) db 0 ;we have to fill 510 bytes and we pad with 0 
dw 0xAA55


buffer:
