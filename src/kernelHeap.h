#pragma once
#include <stddef.h>
void *kmalloc(size_t size);

void kernelHeapInit();
void kfree(void *ptr);
