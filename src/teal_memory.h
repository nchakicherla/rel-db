#ifndef TEAL_MEMORY_H
#define TEAL_MEMORY_H

#include <stdlib.h>
#include <string.h>

void 
teal_set_calloc_impl( void *(*fn)(size_t, size_t));

void
teal_set_free_impl( void (fn)(void *ptr));

void *
__teal_calloc(size_t n, size_t size);

void
__teal_free(void *ptr);

void *
__teal_realloc(void *ptr, size_t size);

void *
__teal_shrink_alloc(void *ptr, size_t bytes);

#endif // TEAL_MEMORY_H
