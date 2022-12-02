#pragma once

#include "idt.h"
struct interruptFrame;
void *isr80hCommand0Sum(struct interruptFrame *frame);
