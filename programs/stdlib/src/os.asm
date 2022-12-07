[BITS 32]

global print:function
global getKey:function
global osMalloc:function

print:
   push ebp
   mov ebp, esp
   push dword[ebp+8]
   mov eax, 1
   int 0x80
   add esp, 4
   pop ebp
   ret

getKey:
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
