#ifndef FELT_DB_H
#define FELT_DB_H

#include <stdint.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include "felt_string.h"

typedef enum {
	ID = 0,
	REF = 1,
	STR,
	CH,
	ITR32,
	ITR64,
	FLT,
	BLN,
	DATE,
	CURR,
	TEXT,
} TABLE_FIELD_TYPE;

typedef struct Felt_Database {
	char *name;
	char *uuid;
	size_t n_tables;
	struct Felt_Table *tables;
} *felt_dbR;

typedef struct Felt_Table {
	char *name;
	char *uuid;
	TABLE_FIELD_TYPE *schema;
	size_t n_bytes_row;
	size_t n_cols;
	size_t n_rows;
	void *data;
} *felt_tabR;

felt_tabR
felt_new_table(char* name, char* schema);

void
felt_free_table(felt_tabR *felt_tabRR);

#endif // FELT_DB_H
