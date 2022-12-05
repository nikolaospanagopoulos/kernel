#pragma once
#include "keyboard.h"

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE 0xAE

int ps2KeyboardInit();
struct keyboard *ps2Init();
uint8_t ps2KeyboardScandcodeToChar(uint8_t scancode);
