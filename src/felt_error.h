#ifndef FELT_ERROR_H
#define FELT_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "felt_memory.h"
#include "felt_string.h"

typedef struct Felt_Maybe_Return {
	int32_t code;
	void* data;
} *felt_maybeR;

typedef struct Felt_Message {
	int32_t code;
	char* msg;
	char* fn;
} *felt_msgR;

felt_maybeR
felt_new_maybe(int32_t code, void* data);

felt_msgR
felt_new_message(int32_t code, char* msg, char* fn);

void
felt_free_maybe(felt_maybeR *felt_maybeRR);

void
felt_free_message(felt_msgR *felt_msgRR);

#endif // FELT_ERROR_H
