#pragma once
#include "idt.h"
void *isr80hCommand4Malloc(struct interruptFrame *frame);
void *isr80hCommand5Free(struct interruptFrame *frame);
