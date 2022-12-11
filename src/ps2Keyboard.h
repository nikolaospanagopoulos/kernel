#pragma once
#include "keyboard.h"
#define PS2_CAPSLOCK 0x3A
#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE 0xAE
#define PS2_KEYBOARD_KEY_RELEASED 0b10000000
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

int ps2KeyboardInit();
struct keyboard *ps2Init();
uint8_t ps2KeyboardScandcodeToChar(uint8_t scancode);
void ps2KeyboardHandleInterrupt();
