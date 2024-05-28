#include "nc_error.h"

/*
typedef struct TL_Message {
	int32_t code;
	char* msg;
	char* fn;
} *tl_msgR;

typedef struct TL_Maybe {
	int32_t code;
	void* data;
} *tl_maybe;
*/

tl_msgR
tl_new_message (int32_t code, char* msg, char* fn) {

	tl_msgR new_msg = tl_impl_calloc (1, sizeof(struct TL_Message));
	new_msg->code = code;
	new_msg->msg = tl_str_dup (msg);
	new_msg->fn = tl_str_dup (fn);
	return new_msg;
}

void
tl_free_message (tl_msgR *tl_msgRR) {

	tl_impl_free ((*tl_msgRR)->msg);
	tl_impl_free ((*tl_msgRR)->fn); 
	tl_impl_free (*tl_msgRR);
	*tl_msgRR = NULL;
	return;
}
/*
void
teal_free_maybe (tl_maybe *teal_maybeRR) {

	if ((*teal_maybeRR)->data) {
		tl_impl_free ((*teal_maybeRR)->data);
	}
	tl_impl_free (*teal_maybeRR);
	*teal_maybeRR = NULL;
	return;
}

tl_maybe
teal_new_maybe (int32_t code, void* data) {

	tl_maybe new_maybe = tl_impl_calloc (1, sizeof(struct TL_Maybe));
	new_maybe->code = code;
	new_maybe->data = data;
	return new_maybe;
}
*/