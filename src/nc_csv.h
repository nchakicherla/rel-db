#ifndef NVC_CSV_H
#define NVC_CSV_H

#include <stdlib.h>

#include "nc_string.h"
#include "nc_file.h"
#include "nc_memory.h"

struct TL_CSV;

struct TL_CSV *  
tl_new_csv (char* csv);

int 
tl_free_csv (struct TL_CSV  **teal_csvRR);

size_t 
tl_csv_get_row_count (struct TL_CSV *teal_csvR);

size_t 
tl_csv_get_col_count (struct TL_CSV *teal_csvR);

char *
tl_csv_get_row_addr (struct TL_CSV *teal_csvR, size_t ind);
/*
void 
teal_debug_print_csv_info (teal_csvR csv);
*/
#endif // NVC_CSV_H
