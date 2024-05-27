#include "teal_memory.h"

void *
(*calloc_impl_fn)(size_t, size_t) = calloc;

void 
(*free_impl_fn)(void *) = free;

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

void *
impl_calloc (size_t n, size_t size) {

	return calloc_impl_fn (n, size);
}

void
impl_free (void *ptr) {
/*
	if (!ptr) {
		return;
	}
*/
	free_impl_fn (ptr);
	return;
}

void *
grow_alloc (void *ptr, size_t new_bytes, size_t old_bytes) {

	void *ptr_out = impl_calloc (new_bytes, 1);
	memcpy (ptr_out, ptr, old_bytes);
	impl_free (ptr);

	return ptr_out;
}

void *
shrink_alloc (void *ptr, size_t bytes) {

	void *ptr_out = impl_calloc (bytes, 1);
	memcpy (ptr_out, ptr, bytes);
	impl_free (ptr);

	return ptr_out;
}
