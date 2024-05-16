#ifndef PM_DB_H
#define PM_DB_H

#include <stdint.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include "pm_string.h"

typedef enum {
	STR = 0,
	ITR32,
	ITR64,
	FLT,
	DATE,
	CURR,
	TEXT,
} TABLE_FIELD_TYPE;

typedef struct PM_Database {
	char *name;
	char *uuid;
	size_t n_tables;
	struct PM_Table *tables;
} *pm_dbR;

typedef struct PM_Table {
	char *name;
	char *uuid;
	TABLE_FIELD_TYPE *schema;
	size_t n_bytes_row;
	size_t n_cols;
	size_t n_rows;
	void *data;
} *pm_tabR;

pm_tabR
pm_new_table(char* name, char* schema);

void
pm_free_table(pm_tabR *pm_tabRR);

#endif // PM_DB_H