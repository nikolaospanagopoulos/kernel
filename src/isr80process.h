#pragma once
#include "idt.h"
void *isr80hCommand6ProcessLoadStart(struct interruptFrame *frame);
void *isr80hCommand7InvokeSystemCommand(struct interruptFrame *frame);
void *isr80hCommand8GetProgramArguments(struct interruptFrame *frame);
