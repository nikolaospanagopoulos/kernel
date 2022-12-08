[BITS 32]
section .asm
global print:function
global osGetKey:function
global osMalloc:function
global osFree:function
global osPutchar:function
global osProcessLoadStart:function

print:
   push ebp
   mov ebp, esp
   push dword[ebp+8]
   mov eax, 1
   int 0x80
   add esp, 4
   pop ebp
   ret

osGetKey:
   push ebp
   mov ebp, esp
   mov eax, 2
   int 0x80
   pop ebp
   ret

osMalloc:
   push ebp
   mov ebp, esp
   mov eax, 4
   push dword[ebp + 8]
   int 0x80
   add esp, 4
   pop ebp
   ret
osFree:
   push ebp,
   mov ebp, esp
   mov eax, 5
   push dword[ebp + 8]
   int 0x80
   add esp, 4
   pop ebp
   ret
osPutchar:
   push ebp
   mov ebp, esp
   mov eax, 3
   push dword[ebp + 8]
   int 0x80
   add esp, 4
   pop ebp
   ret
osProcessLoadStart:
   push ebp
   mov ebp, esp
   mov eax, 6
   push dword[ebp +8]
   int 0x80
   add esp, 4
   pop ebp
   ret
