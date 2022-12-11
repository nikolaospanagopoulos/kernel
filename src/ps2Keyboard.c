#include "ps2Keyboard.h"
#include "idt.h"
#include "io.h"
#include "kernel.h"
#include "task.h"
#include <stddef.h>
#include <stdint.h>

#define PS2_CAPSLOCK 0x3A
static uint8_t keyboardScanSetOne[] = {
    0x00, 0x1B, '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
    '-',  '=',  0x08, '\t', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
    'O',  'P',  '[',  ']',  0x0d, 0x00, 'A',  'S',  'D',  'F',  'G',  'H',
    'J',  'K',  'L',  ';',  '\'', '`',  0x00, '\\', 'Z',  'X',  'C',  'V',
    'B',  'N',  'M',  ',',  '.',  '/',  0x00, '*',  0x00, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',  '8',  '9',
    '-',  '4',  '5',  '6',  '+',  '1',  '2',  '3',  '0',  '.'

};

struct keyboard ps2Keyboard = {.name = "Classic", .init = ps2KeyboardInit};

int ps2KeyboardInit() {
  idtRegisterInterruptCallback(ISR_KEYBOARD_INTERRUPT,
                               ps2KeyboardHandleInterrupt);

  keyboardSetCapsLock(&ps2Keyboard, 0);

  outb(PS2_PORT, PS2_COMMAND_ENABLE);
  return 0;
}

uint8_t ps2KeyboardScandcodeToChar(uint8_t scancode) {
  size_t sizeOfKeyboardSetOne = sizeof(keyboardScanSetOne) / sizeof(uint8_t);

  if (scancode > sizeOfKeyboardSetOne) {
    return 0;
  }

  char c = keyboardScanSetOne[scancode];
  if (keyboardGetCapsLockState(&ps2Keyboard) == 0) {
    if (c >= 'A' && c <= 'Z') {
      c += 32;
    }
  }
  return c;
}

struct keyboard *ps2Init() {
  return &ps2Keyboard;
}

void ps2KeyboardHandleInterrupt() {
  kernelPage();
  uint8_t scancode = 0;
  scancode = insb(KEYBOARD_INPUT_PORT);
  insb(KEYBOARD_INPUT_PORT);

  if (scancode & PS2_KEYBOARD_KEY_RELEASED) {
    return;
  }

  if (scancode == PS2_CAPSLOCK) {
    int oldState = keyboardGetCapsLockState(&ps2Keyboard);
    keyboardSetCapsLock(&ps2Keyboard, oldState == 1 ? 0 : 1);
  }
  uint8_t c = ps2KeyboardScandcodeToChar(scancode);
  if (c != 0) {
    keyboardPush(c);
  }
  taskPage();
}
