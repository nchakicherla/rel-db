#ifndef NVC_MEMORY_H
#define NVC_MEMORY_H

#include <stdlib.h>
#include <string.h>

// 

void 
tl_set_calloc_impl ( void *(fn)(size_t, size_t));

void
tl_set_free_impl (void (fn)(void *ptr));

void *
tl_impl_calloc (size_t n, size_t size);

void
tl_impl_free (void *ptr);

void *
tl_impl_grow_alloc (void *ptr, size_t new_size, size_t old_size);

void *
tl_impl_shrink_alloc (void *ptr, size_t bytes);

#endif // NVC_MEMORY_H