#ifndef NC_MEMORY_H
#define NC_MEMORY_H

#include <stdlib.h>
#include <string.h>

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, old_count, new_count) \
    (type*)reallocate(pointer, sizeof(type) * (old_count), \
        sizeof(type) * (new_count))

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t old_size, size_t new_size);

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

#endif // NC_MEMORY_H
