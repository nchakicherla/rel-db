#ifndef NVC_DB_H
#define NVC_DB_H

//#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
//#include <math.h>
#include <ctype.h>
#include <stddef.h>
//#include <stdarg.h>
//int write_field_CURR (char *value, void *start_addr);

#include "nc_memory.h"
#include "nc_string.h"
#include "nc_csv.h"
// table->n_cols can't exceed SIZE_MAX - 1
//
#define NO_PRIMARY_KEY SIZE_MAX
#define ROW_ID_INVALID SIZE_MAX	

/*
typedef struct Teal_Database {
	
	char *label;
	char *uuid;
	size_t n_tables;
	struct TL_Table *tables;

} *teal_dbR;
*/
struct TL_Table;

typedef struct TL_Reference_Field {
	//char *db_uuid;
	char *tab_uuid;
	size_t row;
	size_t col;

} tl_ref;

void 
tl_print_table (struct TL_Table *tableR);

void *
tl_get_row_addr_by_id (struct TL_Table *tableR, size_t id);

int 
tl_fprint_row (struct TL_Table *tableR, void *addr, FILE* stream);

struct TL_Table * 
tl_new_table (char* label, char* schema, size_t n_cols, size_t primary_index);

void
tl_table_free (struct TL_Table **tableRR);

int
tl_table_set_all_labels (struct TL_Table *tableR, char **labels);

int
tl_table_insert_row (struct TL_Table *tableR, char *row, bool skip_valid);

size_t 
tl_table_load_from_csv (struct TL_Table *teal_tabR, struct TL_CSV *csvR);

void
tl_debug_print_table_info (struct TL_Table *tableR);

#endif // NVC_DB_H
