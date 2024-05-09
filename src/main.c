#include <stdio.h>
#include <stdint.h>

#include "pm_memory.h"
#include "pm_error.h"
#include "pm_string.h"

#define SZ1 100000
#define SZ2  50000

int main(void) {
	
	size_t *new_arr = pm_calloc(SZ1, sizeof(size_t));
	for (size_t i = 0; i < SZ1; i++) {
		new_arr[i] = i + 1;
		printf("%zu\n", i);
	}

	size_t *second_arr = pm_shrinkalloc(new_arr, SZ2 * sizeof(size_t));
	for (size_t i = 0; i < SZ2; i++) {
		printf("%zu: %zu\n", i, second_arr[i]);
	}

	free(second_arr);

	return 0;
}
