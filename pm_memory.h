#ifndef PM_MEMORY_H
#define PM_MEMORY_H

#include <stdlib.h>

void *
pm_calloc(size_t n, size_t size);

void *
pm_realloc(void *ptr, size_t size);

#endif // PM_MEMORY_H