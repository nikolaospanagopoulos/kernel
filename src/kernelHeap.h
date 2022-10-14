#pragma once
#include "memory.h"
#include <stddef.h>
void *kmalloc(size_t size);

void kernelHeapInit();
void kfree(void *ptr);
void *kzalloc(size_t size);
