section .asm
global gdtLoad


gdtLoad:
   mov eax, [esp +4]
   mov [gdt_descriptor + 2], eax
   mov ax, [esp + 8]
   mov [gdt_descriptor], ax
   lgdt [gdt_descriptor]
   ret

section .data
gdt_descriptor:
   dw 0x00;size
   dd 0x00; gdt start address
