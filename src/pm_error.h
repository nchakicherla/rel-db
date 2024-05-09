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

#endif // PM_ERROR_H
