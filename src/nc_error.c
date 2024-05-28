#include "nc_error.h"

tl_msgR
tl_new_message (int32_t code, char* msg, char* fn) {

	tl_msgR new_msg = tl_impl_calloc (1, sizeof(struct TL_Message));
	new_msg->code = code;
	new_msg->msg = tl_new_str_dup (msg);
	new_msg->fn = tl_new_str_dup (fn);
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
