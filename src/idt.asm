section .asm

extern int21h_handler
extern int0h_handler
extern no_interrupt_handler
extern isr80handler
global isr80hWrapper
global int21h
global int0h
global no_interrupt
global idt_load
global enable_interrupts
global disable_interrupts
enable_interrupts:
    sti
	ret
disable_interrupts:
    cli
	ret
idt_load:
    push ebp
	mov ebp, esp
	mov ebx, [ebp + 8]
	lidt [ebx]
	pop ebp
	ret

int21h:
    pushad
	call int21h_handler
	popad
	iret
int0h:
    pushad
	call int0h_handler
	popad
	iret
no_interrupt:
    pushad
	call no_interrupt_handler
	popad
	iret
isr80hWrapper:
   ;push the general purpose registers
   pushad
   ;interupt frame ends
   ;push stack pointer that points to interupt frame
   push esp
   push eax
   call isr80handler
   ;put return address from eax to temp var
   mov dword[tmpRes], eax
   ;restore st pointer
   add esp, 8
   ;restore general purpose registers for userland
   popad
   mov eax, [tmpRes]
   iretd


section .data
tmpRes: dd 0 ;here we store return result from isr80handler

