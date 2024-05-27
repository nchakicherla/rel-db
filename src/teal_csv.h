#ifndef TEAL_CSV_H
#define TEAL_CSV_H

#include <stdlib.h>

#include "teal_string.h"
#include "teal_file.h"
#include "teal_memory.h"

typedef struct Teal_CSV {

	char *source_file;
	bool validated;
	size_t n_rows;
	size_t n_cols;

	char **labels;
	char **rows;

} *teal_csvR;

teal_csvR 
teal_new_csv (char* csv);

int 
teal_free_csv (teal_csvR *teal_csvRR);
/*
void 
teal_debug_print_csv_info (teal_csvR csv);
*/
#endif // TEAL_CSV_H