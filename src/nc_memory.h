#ifndef NVC_MEMORY_H
#define NVC_MEMORY_H

#include <stdlib.h>
#include <string.h>

// 

void 
teal_set_calloc_impl ( void *(fn)(size_t, size_t));

void
teal_set_free_impl (void (fn)(void *ptr));

void *
impl_calloc (size_t n, size_t size);

void
impl_free (void *ptr);

void *
impl_grow_alloc (void *ptr, size_t new_size, size_t old_size);

void *
impl_shrink_alloc (void *ptr, size_t bytes);

#endif // NVC_MEMORY_H
