#ifndef FELT_DB_H
#define FELT_DB_H

#include <stdint.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <stdbool.h>
//#include <stdarg.h>

#include "felt_string.h"

// table->n_cols can't exceed SIZE_MAX - 1
//

#define NO_PRIMARY	SIZE_MAX

typedef enum {
	STR = 0,
	ITR32,
	ITR64,
	FLT,
	BLN,
	DATE,
	CURR,
	CH,
	ID,
	REF,
} TABLE_FIELD_TYPE;

typedef struct Felt_Database {
	char *label;
	char *uuid;
	size_t n_tables;
	struct Felt_Table *tables;
} *felt_dbR;

typedef struct Felt_Table {
	char *label;
	char *uuid;
	TABLE_FIELD_TYPE *schema;
	char **col_labels;
	bool primary_specified;
	size_t primary;
	size_t n_bytes_row;
	size_t n_cols;
	size_t n_rows;
	void *data;
} *felt_tabR;

typedef struct Felt_Reference {
	char *db_uuid;
	char *tab_uuid;
	size_t row;
	size_t col;
} *felt_refR;

felt_tabR
felt_new_table(char* label, size_t primary_index, char* schema);

void
felt_free_table(felt_tabR *felt_tabRR);
/*
int
felt_update_table_column_labelsV(int count, ...);
*/
int
felt_update_table_column_labels(felt_tabR table, char **labels);

int
felt_insert_row(felt_tabR table, char *row);

#endif // FELT_DB_H
