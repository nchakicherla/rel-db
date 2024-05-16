#include "felt_error.h"

/*
typedef struct Felt_Message {
	int32_t code;
	char* msg;
	char* fn;
} *felt_msgR;

typedef struct Felt_Maybe_Return {
	int32_t code;
	void* data;
} *felt_maybeR;
*/

felt_msgR
felt_new_message(int32_t code, char* msg, char* fn) {
	felt_msgR new_msg = felt_calloc(1, sizeof(struct Felt_Message));
	new_msg->code = code;
	new_msg->msg = felt_str_dup(msg);
	new_msg->fn = felt_str_dup(fn);
	return new_msg;
}

felt_maybeR
felt_new_maybe(int32_t code, void* data) {
	felt_maybeR new_maybe = felt_calloc(1, sizeof(struct Felt_Maybe_Return));
	new_maybe->code = code;
	new_maybe->data = data;
	return new_maybe;
}

void
felt_free_message(felt_msgR *felt_msgRR) {
	free((*felt_msgRR)->msg);
	free((*felt_msgRR)->fn);
	free(*felt_msgRR);
	*felt_msgRR = NULL;
	return;
}

void
felt_free_maybe(felt_maybeR *felt_maybeRR) {
	if ((*felt_maybeRR)->data) {
		free((*felt_maybeRR)->data);
	}
	free(*felt_maybeRR);
	*felt_maybeRR = NULL;
	return;
}
