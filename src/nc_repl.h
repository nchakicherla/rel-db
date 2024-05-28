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
} tl_tok_type;

typedef struct TL_Token {
	tl_tok_type type;
	void *data;
} *tl_tokR;

tl_tokR
tl_new_tok (tl_tok_type type, void *data);

#endif // NVC_INTERPRETER_H
