#include "nc_csv.h"

typedef struct TL_CSV {

	char *source_file;
	size_t n_rows;
	size_t n_cols;

	char **labels;
	char **rows;

} *tl_csvR;

tl_csvR 
tl_new_csv (char* file_name) {

	char* buffer = tl_new_str_from_file (file_name);
	if (NULL == buffer) {
		return NULL;
	}

	tl_csvR ret = tl_calloc (1, sizeof(struct TL_CSV));

	ret->rows = tl_new_str_arr (buffer, "\n", &ret->n_rows);	// split csv by newlines
	ret->labels = tl_new_str_arr (ret->rows[0], ",", &ret->n_cols); // split first line by commas
	
	printf("n_rows in csv: %zu\n", ret->n_rows);

	char **shifted_rows = tl_calloc (ret->n_rows - 1, sizeof(char *));

	for (size_t i = 1, j = 0; i < ret->n_rows; i++, j++) {
		shifted_rows[j] = ret->rows[i];
	}
	tl_free (ret->rows[0]);
	tl_free (ret->rows);
	ret->rows = shifted_rows;
	ret->n_rows--; // decrement after shifting rows

	ret->source_file = tl_new_str_dup (file_name);

	tl_free (buffer);
	return ret;
}

int 
tl_free_csv (tl_csvR *teal_csvRR) {

	for (size_t i = 0; i < (*teal_csvRR)->n_rows; i++) {
		tl_free ((*teal_csvRR)->rows[i]);
	}
	for (size_t i = 0; i < (*teal_csvRR)->n_cols; i++) {
		tl_free ((*teal_csvRR)->labels[i]);
	}
	tl_free ((*teal_csvRR)->source_file);
	tl_free ((*teal_csvRR)->rows);
	tl_free ((*teal_csvRR)->labels);
	tl_free (*teal_csvRR);
	*teal_csvRR = NULL;
	return 0;
}

size_t 
tl_csv_get_row_count (struct TL_CSV *tl_csvR) {

	return tl_csvR->n_rows;
}

size_t 
tl_csv_get_col_count (struct TL_CSV *tl_csvR) {

	return tl_csvR->n_cols;
}

char *
tl_csv_get_row_addr (struct TL_CSV *tl_csvR, size_t id) {

	if (id > tl_csvR->n_rows || tl_csvR->n_rows == 0) {
		return NULL;
	}
	return tl_csvR->rows[ id - 1 ];
}
