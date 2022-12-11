[BITS 32]

section .asm

global restoreGeneralPurposeRegisters
global taskReturn
global userRegisters

userRegisters:
   mov ax, 0x23
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   ret

taskReturn:
   ;push the data segment
   ;push the stack address
   ;push the flags
   ;push the code segment
   ;push ip

mov ebp, esp
   mov ebx , [esp + 4]
   push dword [ebx + 44]
   push dword [ebx + 40]
   pushf
   pop eax,
   or eax, 0x200
   push eax

   push dword [ebx + 32]
   push dword [ebx + 28]

   mov ax, [ebx + 44]
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   push dword [ebp + 4]
   call restoreGeneralPurposeRegisters
   add esp, 4
   iretd




restoreGeneralPurposeRegisters:
   push ebp
   mov ebp, esp
   mov ebx, [esp + 8]
   mov edi, [ebx]
   mov esi, [ebx + 4]
   mov ebp, [ebx + 8]
   mov edx, [ebx + 16]
   mov ecx, [ebx + 20]
   mov eax, [ebx + 24]
   mov ebx, [ebx + 12]
   add esp, 4
   ret
