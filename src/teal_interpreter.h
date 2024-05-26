#ifndef TEAL_INTERPRETER_H
#define TEAL_INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "teal_string.h"

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

#endif // TEAL_INTERPRETER_H
