#include "isr80h.h"
#include "idt.h"
#include "isr80hIo.h"
#include "isr80heap.h"
#include "misc.h"

void isr80hRegisterCommands() {

  isr80RegisterCommand(SYSTEM_COMMAND_0_SUM, isr80hCommand0Sum);
  isr80RegisterCommand(SYSTEM_COMMAND_1_PRINT, isr80hCommand1Print);
  isr80RegisterCommand(SYSTEM_COMMAND_2_GETKEY, isr80hCommand2GetKey);
  isr80RegisterCommand(SYSTEM_COMMAND_3_PUTCHAR, isr80hCommand3PutChar);
  isr80RegisterCommand(SYSTEM_COMMAND_4_MALLOC, isr80hCommand4Malloc);
}
