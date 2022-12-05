#pragma once

struct interruptFrame;

void *isr80hCommand1Print(struct interruptFrame *frame);

void *isr80hCommand2GetKey(struct interruptFrame *frame);

void *isr80hCommand3PutChar(struct interruptFrame *frame);
