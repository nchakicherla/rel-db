#ifndef Felt_INTERPRETER_H
#define Felt_INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "felt_string.h"

typedef enum
{
	TOK_INT,
	TOK_FLT,
	TOK_STR,
	TOK_OPR,
} Felt_TOK_TYPE;

typedef struct felt_Token {
	Felt_TOK_TYPE type;
	void *data;
} *felt_tokR;

felt_tokR
felt_new_tok(Felt_TOK_TYPE type, void *data);

#endif // Felt_INTERPRETER_H
