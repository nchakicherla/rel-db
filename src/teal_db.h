#ifndef FELT_DB_H
#define FELT_DB_H

#include <stdint.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <ctype.h>
//#include <stdarg.h>

#include "teal_string.h"
// table->n_cols can't exceed SIZE_MAX - 1
//
#define NO_PRIMARY							SIZE_MAX


typedef enum {
	STR = 0,
	ITR32,
	ITR64,
	DBL,
	BLN,
	DATE,
	CURR,
	CH,
	ID,
	REF,
} TABLE_FIELD_TYPE;

typedef struct Teal_Database {
	char *label;
	char *uuid;
	size_t n_tables;
	struct Teal_Table *tables;
} *teal_dbR;

typedef struct Teal_Table {
	char *label;
	char *uuid;
	TABLE_FIELD_TYPE *schema;
	char **col_labels;
	bool primary_specified;
	size_t primary;
	size_t n_bytes_row;
	size_t n_cols;
	size_t n_rows;
	void *bytes;
} *teal_tabR;

typedef struct Teal_Reference_Field {
	char *db_uuid;
	char *tab_uuid;
	size_t row;
	size_t col;
} *teal_ref_fieldR;

teal_tabR
teal_new_table(char* label, size_t primary_index, char* schema);

void
teal_free_table(teal_tabR *teal_tabRR);
/*
int
teal_update_table_column_labelsV(int count, ...);
*/
int
teal_update_table_column_labels(teal_tabR table, char **labels);

int
teal_insert_row_from_chars(teal_tabR table, char *row);

int
teal_set_row_input_valid_fns(void);

int
teal_set_write_row_field_fns(void);

#endif // FELT_DB_H
