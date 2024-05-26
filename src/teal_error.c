#include "teal_error.h"

/*
typedef struct Teal_Message {
	int32_t code;
	char* msg;
	char* fn;
} *teal_msgR;

typedef struct Teal_Maybe_Return {
	int32_t code;
	void* data;
} *teal_maybeR;
*/

teal_msgR
teal_new_message(int32_t code, char* msg, char* fn) {
	teal_msgR new_msg = __teal_calloc(1, sizeof(struct Teal_Message));
	new_msg->code = code;
	new_msg->msg = teal_str_dup(msg);
	new_msg->fn = teal_str_dup(fn);
	return new_msg;
}

teal_maybeR
teal_new_maybe(int32_t code, void* data) {
	teal_maybeR new_maybe = __teal_calloc(1, sizeof(struct Teal_Maybe_Return));
	new_maybe->code = code;
	new_maybe->data = data;
	return new_maybe;
}

void
teal_free_message(teal_msgR *teal_msgRR) {
	free((*teal_msgRR)->msg);
	free((*teal_msgRR)->fn);
	free(*teal_msgRR);
	*teal_msgRR = NULL;
	return;
}

void
teal_free_maybe(teal_maybeR *teal_maybeRR) {
	if ((*teal_maybeRR)->data) {
		free((*teal_maybeRR)->data);
	}
	free(*teal_maybeRR);
	*teal_maybeRR = NULL;
	return;
}
