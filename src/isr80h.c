#include "isr80h.h"
#include "idt.h"
#include "isr80hIo.h"
#include "isr80heap.h"
#include "isr80process.h"
#include "misc.h"

void isr80hRegisterCommands() {

  isr80RegisterCommand(SYSTEM_COMMAND_0_SUM, isr80hCommand0Sum);
  isr80RegisterCommand(SYSTEM_COMMAND_1_PRINT, isr80hCommand1Print);
  isr80RegisterCommand(SYSTEM_COMMAND_2_GETKEY, isr80hCommand2GetKey);
  isr80RegisterCommand(SYSTEM_COMMAND_3_PUTCHAR, isr80hCommand3PutChar);
  isr80RegisterCommand(SYSTEM_COMMAND_4_MALLOC, isr80hCommand4Malloc);
  isr80RegisterCommand(SYSTEM_COMMAND_5_FREE, isr80hCommand5Free);
  isr80RegisterCommand(SYSTEM_COMMAND_6_PROCESS_LOAD_START,
                       isr80hCommand6ProcessLoadStart);
  isr80RegisterCommand(SYSTEM_COMMAND_7_INVOKE_SYSTEM_COMMAND,
                       isr80hCommand7InvokeSystemCommand);
  isr80RegisterCommand(SYSTEM_COMMAND_8_GET_PROGRAM_ARGUMENTS,
                       isr80hCommand8GetProgramArguments);
  isr80RegisterCommand(SYSTEM_COMMAND_9_EXIT, isr80hCommand9Exit);
}
