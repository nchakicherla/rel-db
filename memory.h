#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

void *
wcalloc(size_t n, size_t size);

void *
wrealloc(void *ptr, size_t size);

#endif // MEMORY_H