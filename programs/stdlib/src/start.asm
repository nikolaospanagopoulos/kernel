[BITS 32]

section .asm

global _start
extern cStart
extern osExit
_start:
   call cStart
   call osExit
   ret
