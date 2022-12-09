[BITS 32]

section .asm

global _start
extern cStart

_start:
   call cStart
   ret
