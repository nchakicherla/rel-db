#ifndef NVC_ERROR_H
#define NVC_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "nc_memory.h"
#include "nc_string.h"

typedef struct TL_Maybe {
	void* data;
	int32_t code;
} tl_maybe;

typedef struct TL_Message {
	int32_t code;
	char* msg;
	char* fn;
} *tl_msgR;
/*
tl_maybe
teal_new_maybe (int32_t code, void* data);
*/
tl_msgR
tl_new_message (int32_t code, char* msg, char* fn);
/*
void
teal_free_maybe (tl_maybe *teal_maybeRR);
*/
void
tl_free_message (tl_msgR *tl_msgRR);

#endif // NVC_ERROR_H
