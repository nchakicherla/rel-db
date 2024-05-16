#ifndef FELT_MEMORY_H
#define FELT_MEMORY_H

#include <stdlib.h>
#include <string.h>

void *
felt_calloc(size_t n, size_t size);

void
felt_free(void *ptr);

void *
felt_grow_alloc(void *ptr, size_t size);

void *
felt_shrink_alloc(void *ptr, size_t bytes);

#endif // FELT_MEMORY_H
