#include "nc_memory.h"

void *
(*tl_impl_calloc)(size_t, size_t) = calloc;

void 
(*tl_impl_free)(void *) = free;

void
tl_set_calloc_impl_fn (void *(*fn)(size_t, size_t)) {

	tl_impl_calloc = fn;
	return;
}

void
tl_set_free_impl_fn (void (*fn)(void *ptr)) {

	tl_impl_free = fn;
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
