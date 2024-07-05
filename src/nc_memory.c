#include "nc_memory.h"

void *
(*tl_calloc)(size_t, size_t) = calloc;

void 
(*tl_free)(void *) = free;

void
tl_set_calloc_fn(void *(*fn)(size_t, size_t)) {

	tl_calloc = fn;
	return;
}

void
tl_set_free_fn(void (*fn)(void *ptr)) {

	tl_free = fn;
	return;
}

void *
tl_grow_alloc(void *ptr, size_t n_bytes, size_t n_bytes_old) {

	void *ptr_out = tl_calloc(n_bytes, 1);
	memcpy(ptr_out, ptr, n_bytes_old);
	
	tl_free(ptr);

	return ptr_out;
}

void *
tl_shrink_alloc(void *ptr, size_t n_bytes) {

	void *ptr_out = tl_calloc(n_bytes, 1);
	memcpy (ptr_out, ptr, n_bytes);
	
	tl_free(ptr);

	return ptr_out;
}
