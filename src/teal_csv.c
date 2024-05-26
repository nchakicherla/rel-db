#include "teal_csv.h"

teal_csvR 
teal_new_csv (char* csv) {

	char* buffer = teal_new_str_from_file (csv);
	if (NULL == buffer) {
		return NULL;
	}

	size_t n_rows = 0;
	char **split = teal_new_arr_from_str (buffer, "\n", &n_rows);

	teal_csvR ret = __teal_calloc (1, sizeof(struct Teal_Raw_CSV));
	ret->source_file = teal_str_dup (csv);
	ret->n_rows = n_rows;
	ret->rows = split;

	__teal_free (buffer);
	return ret;
}

int 
teal_free_csv (teal_csvR *teal_csvRR) {

	for (size_t i = 0; i < (*teal_csvRR)->n_rows; i++) {
		__teal_free ( (*teal_csvRR)->rows[i] );
	}
	__teal_free ((*teal_csvRR)->source_file);
	__teal_free ((*teal_csvRR)->rows);
	__teal_free (*teal_csvRR);
	*teal_csvRR = NULL;
	return 0;
}
