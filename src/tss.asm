section .asm

global tssLoad

tssLoad:
   push ebp
   mov ebp, esp
   mov ax, [ebp + 8]
   ltr ax
   pop ebp
   ret
