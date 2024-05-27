#ifndef TEAL_ERROR_H
#define TEAL_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "teal_memory.h"
#include "teal_string.h"

typedef struct Teal_Maybe {
	void* data;
	int32_t code;
} teal_maybe;

typedef struct Teal_Message {
	int32_t code;
	char* msg;
	char* fn;
} *teal_msgR;
/*
teal_maybe
teal_new_maybe (int32_t code, void* data);
*/
teal_msgR
teal_new_message (int32_t code, char* msg, char* fn);
/*
void
teal_free_maybe (teal_maybe *teal_maybeRR);
*/
void
teal_free_message (teal_msgR *teal_msgRR);

#endif // TEAL_ERROR_H
