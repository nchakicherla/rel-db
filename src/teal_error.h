#ifndef FELT_ERROR_H
#define FELT_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "teal_memory.h"
#include "teal_string.h"

typedef struct Teal_Maybe_Return {
	int32_t code;
	void* data;
} *teal_maybeR;

typedef struct Teal_Message {
	int32_t code;
	char* msg;
	char* fn;
} *teal_msgR;

teal_maybeR
teal_new_maybe(int32_t code, void* data);

teal_msgR
teal_new_message(int32_t code, char* msg, char* fn);

void
teal_free_maybe(teal_maybeR *teal_maybeRR);

void
teal_free_message(teal_msgR *teal_msgRR);

#endif // FELT_ERROR_H
