#include <stdio.h>
#include <stdint.h>

#include "memory.h"

/*
typedef enum EXIT_CODE {
	RETURN_SUCCESS,
	RETURN_FAILURE
} EXIT_CODE;

typedef struct Object {
	size_t x;
} *ObjectRef;

EXIT_CODE
initObject(size_t val, ObjectRef *obj) {
	*obj = wcalloc(1, sizeof(struct Object));
	(*obj)->x = val;
	printf("in init: %zu\n", (*obj)->x);
	return RETURN_SUCCESS;
}
*/

int main(void) {
	
	int32_t *test_ptr = wcalloc(1, sizeof(int32_t));
	test_ptr = wrealloc(test_ptr, 2 * sizeof(int32_t));
	
	free(test_ptr);
	return 0;
}