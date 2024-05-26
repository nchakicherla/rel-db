#include "teal_memory.h"

void *(*calloc_impl_fn)(size_t, size_t) = NULL;
void (*free_impl_fn)(void *) = NULL;

void
teal_set_calloc_impl ( void *(*fn)(size_t, size_t)) {
	calloc_impl_fn = fn;
	return;
}

void
teal_set_free_impl ( void (*fn)(void *ptr)) {
	free_impl_fn = fn;
	return;
}

/*
void *
__teal_calloc(size_t n, size_t size) {

	void *ptr_out = NULL;
	if(!(ptr_out = calloc(n, size))) {
		exit(EXIT_FAILURE);
	}
	return ptr_out;
}
*/
void *
__teal_calloc (size_t n, size_t size) {
	void *ptr_out = NULL;
	ptr_out = calloc_impl_fn (n, size);
	return ptr_out;
}

void
__teal_free (void *ptr) {

	if (!ptr) {
		return;
	}
	//free(ptr);
	free_impl_fn (ptr);
	return;
}

void *
__teal_realloc (void *ptr, size_t bytes) {

	void *ptr_out = NULL;
	if(!(ptr_out = realloc (ptr, bytes))) {
		exit (EXIT_FAILURE);
	}
	return ptr_out;
}

void *
__teal_shrink_alloc (void *ptr, size_t bytes) {

	void *ptr_out = __teal_calloc (bytes, 1);
	memcpy (ptr_out, ptr, bytes);
	free (ptr);

	return ptr_out;
}
