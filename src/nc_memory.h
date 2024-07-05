#ifndef NVC_MEMORY_H
#define NVC_MEMORY_H

#include <stdlib.h>
#include <string.h>

extern void *
(*tl_calloc) (size_t n, size_t size);

extern void
(*tl_free) (void *ptr);

void 
tl_set_calloc_fn(void *(fn)(size_t, size_t));

void
tl_set_free_fn(void (fn)(void *ptr));

void *
tl_grow_alloc(void *ptr, size_t new_size, size_t old_size);

void *
tl_shrink_alloc(void *ptr, size_t bytes);

#endif // NVC_MEMORY_H
