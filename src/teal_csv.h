#ifndef TEAL_CSV_H
#define TEAL_CSV_H

#include <stdlib.h>

#include "teal_string.h"
#include "teal_file.h"
#include "teal_memory.h"

typedef struct Teal_Raw_CSV {
	char *source_file;
	size_t n_rows;
	char** rows;
} *teal_csvR;

teal_csvR 
teal_new_csv (char* csv);

int 
teal_free_csv (teal_csvR *teal_csvRR);

#endif // TEAL_CSV_H