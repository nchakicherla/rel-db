#ifndef PM_MEMORY_H
#define PM_MEMORY_H

#include <stdlib.h>
#include <string.h>

void *
pm_calloc(size_t n, size_t size);

void
pm_free(void *ptr);

void *
pm_grow_alloc(void *ptr, size_t size);

void *
pm_shrink_alloc(void *ptr, size_t bytes);

#endif // PM_MEMORY_H
