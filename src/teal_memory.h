#ifndef TEAL_MEMORY_H
#define TEAL_MEMORY_H

#include <stdlib.h>
#include <string.h>

void *
teal_calloc(size_t n, size_t size);

void
teal_free(void *ptr);

void *
teal_grow_alloc(void *ptr, size_t size);

void *
teal_shrink_alloc(void *ptr, size_t bytes);

#endif // TEAL_MEMORY_H
