#ifndef NC_DB_H
#define NC_DB_H

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
#define TL_NO_PRIMARY SIZE_MAX
//#define ROW_ID_INVALID SIZE_MAX	

struct TL_Table;

void 
tl_print_table(struct TL_Table *tableR);

void *
tl_tab_get_row_addr(struct TL_Table *tableR, size_t id);

int 
tl_tab_fprint_row(struct TL_Table *tableR, void *addr, FILE* stream);

struct TL_Table * 
tl_tab_new(char* label, char* schema, size_t n_cols, size_t primary_index);

void
tl_tab_free(struct TL_Table **tableRR);

int
tl_tab_set_labels(struct TL_Table *tableR, char **labels);

int
tl_tab_insert_row(struct TL_Table *tableR, char *row, bool skip_valid);

size_t 
tl_tab_load_from_csv(struct TL_Table *tl_tabR, struct TL_CSV *csvR);

void
tl_tab_print_info(struct TL_Table *tableR);

#endif // NC_DB_H
