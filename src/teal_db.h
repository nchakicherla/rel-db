#ifndef TEAL_DB_H
#define TEAL_DB_H

#include <stdint.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <ctype.h>
#include <stddef.h>
//#include <stdarg.h>
int teal_write_field_input_CURR(char *value, void *start_addr);

#include "teal_string.h"
// table->n_cols can't exceed SIZE_MAX - 1
//
#define NO_PRIMARY_KEY				SIZE_MAX
#define ROW_ID_INVALID				SIZE_MAX		

typedef enum {
	STR = 0,
	ITR32,
	ITR64,
	DBL,
	BLN,
	DATE,
	CURR,
	CH,
	ROW_ID,
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
	size_t *field_offsets;
	char **col_labels;
	bool primary_specified;
	size_t primary;
	size_t n_bytes_schema;
	size_t n_bytes_row;
	size_t n_cols;
	size_t n_rows;
	size_t bytes_avail;
	size_t bytes_used;
	void *bytes;
} *teal_tabR;

typedef struct Teal_Reference_Field {
	//char *db_uuid;
	char *tab_uuid;
	size_t row;
	size_t col;
} teal_ref_field;

teal_tabR
teal_new_table(char* label, size_t primary_index, char* schema);

void
teal_free_table(teal_tabR *teal_tabRR);

void
teal_free_table_data(teal_tabR table);

int
teal_update_table_column_labels(teal_tabR table, char **labels);

int
teal_insert_row_from_chars(teal_tabR table, char *row);

int
teal_set_row_input_valid_fns(void);

int
teal_set_write_row_field_fns(void);

int
teal_grow_table_bytes(teal_tabR table);

#endif // TEAL_DB_H
