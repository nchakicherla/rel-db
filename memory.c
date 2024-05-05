#include "memory.h"

void *
wcalloc(size_t n, size_t size) {
	void *ptr_out = NULL;
	if(!(ptr_out = calloc(n, size))) {
		exit(EXIT_FAILURE);
	}
	return ptr_out;
}

void *
wrealloc(void *ptr, size_t size) {
	void *ptr_out = NULL;
	if(!(ptr_out = realloc(ptr, size))) {
		exit(EXIT_FAILURE);
	}
	return ptr_out;
}