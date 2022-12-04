#include "isr80h.h"
#include "idt.h"
#include "isr80hIo.h"
#include "misc.h"

void isr80hRegisterCommands() {

  isr80RegisterCommand(SYSTEM_COMMAND_0_SUM, isr80hCommand0Sum);
  isr80RegisterCommand(SYSTEM_COMMAND_1_PRINT, isr80hCommand1Print);
}
