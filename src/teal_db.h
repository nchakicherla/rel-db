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
//int write_field_CURR (char *value, void *start_addr);

#include "teal_memory.h"
#include "teal_string.h"
#include "teal_csv.h"
// table->n_cols can't exceed SIZE_MAX - 1
//
#define NO_PRIMARY_KEY SIZE_MAX
#define ROW_ID_INVALID SIZE_MAX	

/*
typedef struct Teal_Database {
	
	char *label;
	char *uuid;
	size_t n_tables;
	struct Teal_Table *tables;

} *teal_dbR;
*/
struct Teal_Table;

typedef struct Teal_Reference_Field {
	//char *db_uuid;
	char *tab_uuid;
	size_t row;
	size_t col;

} teal_ref;

void 
teal_print_table (struct Teal_Table *tableR);

void *
teal_get_row_addr (struct Teal_Table *tableR, size_t ind);

void
teal_print_row_at_addr (struct Teal_Table *tableR, void *addr);

struct Teal_Table * 
teal_new_table (char* label, char* schema, size_t n_cols, size_t primary_index);

void
teal_table_free (struct Teal_Table **tableRR);

int
teal_table_update_labels (struct Teal_Table *tableR, char **labels);

int
teal_table_insert_row (struct Teal_Table *tableR, char *row);

void
teal_debug_print_table_info (struct Teal_Table *tableR);

#endif // TEAL_DB_H
