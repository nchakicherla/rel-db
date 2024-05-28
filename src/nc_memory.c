#include "nc_memory.h"

void *
(*calloc_impl_fn)(size_t, size_t) = calloc;

void 
(*free_impl_fn)(void *) = free;

void
tl_set_calloc_impl ( void *(*fn)(size_t, size_t)) {

	calloc_impl_fn = fn;
	return;
}

void
tl_set_free_impl ( void (*fn)(void *ptr)) {

	free_impl_fn = fn;
	return;
}

void *
tl_impl_calloc (size_t n, size_t size) {

	return calloc_impl_fn (n, size);
}

void
tl_impl_free (void *ptr) {
/*
	if (!ptr) {
		return;
	}
*/
	free_impl_fn (ptr);
	return;
}

void *
tl_impl_grow_alloc (void *ptr, size_t n_bytes, size_t n_bytes_old) {

	void *ptr_out = tl_impl_calloc (n_bytes, 1);
	memcpy (ptr_out, ptr, n_bytes_old);
	
	tl_impl_free (ptr);

	return ptr_out;
}

void *
tl_impl_shrink_alloc (void *ptr, size_t n_bytes) {

	void *ptr_out = tl_impl_calloc (n_bytes, 1);
	memcpy (ptr_out, ptr, n_bytes);
	
	tl_impl_free (ptr);

	return ptr_out;
}
