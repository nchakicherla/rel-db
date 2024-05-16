#include "felt_memory.h"

void *
felt_calloc(size_t n, size_t size) {
	void *ptr_out = NULL;
	if(!(ptr_out = calloc(n, size))) {
		exit(EXIT_FAILURE);
	}
	return ptr_out;
}

void
felt_free(void *ptr) {
	if (!ptr) return;
	free(ptr);
	return;
}

void *
felt_grow_alloc(void *ptr, size_t bytes) {
	void *ptr_out = NULL;
	if(!(ptr_out = realloc(ptr, bytes))) {
		exit(EXIT_FAILURE);
	}
	return ptr_out;
}

void *
felt_shrink_alloc(void *ptr, size_t bytes) {
	void *ptr_out = felt_calloc(bytes, 1);
	memcpy(ptr_out, ptr, bytes);
	free(ptr);
	return ptr_out;
}
