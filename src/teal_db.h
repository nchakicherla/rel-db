#ifndef TEAL_DB_H
#define TEAL_DB_H

#include <stdint.h>
#include <inttypes.h>
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
//int teal_write_field_input_CURR (char *value, void *start_addr);

#include "teal_memory.h"
#include "teal_string.h"
#include "teal_csv.h"
// table->n_cols can't exceed SIZE_MAX - 1
//
#define NO_PRIMARY_KEY SIZE_MAX
#define ROW_ID_INVALID SIZE_MAX	

typedef enum {
	_ROW_ID = 0,
	STR,
	ITR32,
	ITR64,
	FLT,
	BLN,
	DATE,
	CURR,
	CH,
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

	bool has_primary;
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
} teal_field_reference;

void *
teal_get_row_addr (teal_tabR table, size_t ind);

void
teal_print_row_at_addr (teal_tabR table, void *addr);

teal_tabR
teal_new_table (char* label, char* schema, size_t primary_index, size_t n_cols);

void
teal_table_free (teal_tabR *teal_tabRR);

void
teal_table_free_str_allocs (teal_tabR table);

int
teal_table_update_labels (teal_tabR table, char **labels);

int
teal_table_insert_row (teal_tabR table, char *row);

int
teal_table_grow_bytes (teal_tabR table);

int
teal_set_validate_input_fnptrs (void);

int
teal_set_write_row_field_fns (void);

void
teal_debug_print_table_info (teal_tabR table);

#endif // TEAL_DB_H
