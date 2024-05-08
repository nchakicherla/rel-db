#include "main.h"

int main(void) {
	
	int32_t *test_ptr = pm_calloc(1, sizeof(int32_t));
	test_ptr = pm_realloc(test_ptr, 2 * sizeof(int32_t));
	
	free(test_ptr);
	return 0;
}