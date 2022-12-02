#include "isr80h.h"
#include "idt.h"
#include "misc.h"

void isr80hRegisterCommands() {

  isr80RegisterCommand(SYSTEM_COMMAND_0_SUM, isr80hCommand0Sum);
}
