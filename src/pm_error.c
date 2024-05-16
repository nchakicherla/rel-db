#include "pm_error.h"

/*
typedef struct PM_Message {
	int32_t code;
	char* msg;
	char* fn;
} *pm_msgR;

typedef struct PM_Maybe_Return {
	int32_t code;
	void* data;
} *pm_maybeR;
*/

pm_maybeR
pm_new_maybe(int32_t code, void* data) {
	pm_maybeR new_maybe = pm_calloc(1, sizeof(struct PM_Maybe_Return));
	new_maybe->code = code;
	new_maybe->data = data;
	return new_maybe;
}

void
pm_free_maybe(pm_maybeR *pm_maybeRR) {
	if ((*pm_maybeRR)->data) {
		free((*pm_maybeRR)->data);
	}
	free(*pm_maybeRR);
	*pm_maybeRR = NULL;
	return;
}

pm_msgR
pm_new_msg(int32_t code, char* msg, char* fn) {
	pm_msgR new_msg = pm_calloc(1, sizeof(struct PM_Message));
	new_msg->code = code;
	new_msg->msg = pm_str_dup(msg);
	new_msg->fn = pm_str_dup(fn);
	return new_msg;
}

void
pm_free_msg(pm_msgR *pm_msgRR) {
	free((*pm_msgRR)->msg);
	free((*pm_msgRR)->fn);
	free(*pm_msgRR);
	*pm_msgRR = NULL;
	return;
}
