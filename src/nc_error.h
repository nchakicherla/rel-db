#ifndef NC_ERROR_H
#define NC_ERROR_H

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

tl_msgR
tl_new_message(int32_t code, char* msg, char* fn);

void
tl_free_message(tl_msgR *tl_msgRR);

#endif // NC_ERROR_H
