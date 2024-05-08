#ifndef PM_ERROR_H
#define PM_ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include "pm_memory.h"

typedef struct PM_Message {
	char* fn;
	char* msg;
	int32_t code;
} *pm_msgR;
/*
pm_msg_ref
pm_new_msg(char* fn, char* msg, int32_t code);
*/
#endif // PM_ERROR_H
