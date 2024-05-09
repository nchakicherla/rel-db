#ifndef PM_MEMORY_H
#define PM_MEMORY_H

#include <stdlib.h>
#include <string.h>

void *
pm_calloc(size_t n, size_t size);

void *
pm_growalloc(void *ptr, size_t size);

void *
pm_shrinkalloc(void *ptr, size_t bytes);

#endif // PM_MEMORY_H
