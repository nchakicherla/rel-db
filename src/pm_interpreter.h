#ifndef PM_INTERPRETER_H
#define PM_INTERPRETER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "pm_string.h"

typedef enum
{
	INT,
	FLT,
	STR,
	OPR,
} PM_TOK_TYPE;

typedef struct PM_Token {
	PM_TOK_TYPE type;
	void *data;
} *pm_tokR;

pm_tokR
pm_new_tok(PM_TOK_TYPE type, void *data);

#endif // PM_INTERPRETER_H
