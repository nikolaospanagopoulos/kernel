#include "stdlib.h"
#include "os.h"

void *malloc(size_t size) { return osMalloc(size); }
void free(void *ptr) {}
