#ifndef NVC_INTERPRETER_H
#define NVC_INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "nc_string.h"

typedef enum
{
	TOK_INT,
	TOK_DBL,
	TOK_STR,
	TOK_OPR,
} FELT_TOK_TYPE;

typedef struct teal_Token {
	FELT_TOK_TYPE type;
	void *data;
} *teal_tokR;

teal_tokR
teal_new_tok (FELT_TOK_TYPE type, void *data);

#endif // NVC_INTERPRETER_H
