#pragma once

enum systemCommands {
  SYSTEM_COMMAND_0_SUM,
  SYSTEM_COMMAND_1_PRINT,
  SYSTEM_COMMAND_2_GETKEY,
  SYSTEM_COMMAND_3_PUTCHAR
};
void isr80hRegisterCommands();
