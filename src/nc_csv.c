#include "nc_csv.h"

typedef struct TL_CSV {

	char *source_file;
	bool validated;
	size_t n_rows;
	size_t n_cols;

	char **labels;
	char **rows;

} *teal_csvR;

teal_csvR 
tl_new_csv (char* csv) {

	char* buffer = tl_new_str_from_file (csv);
	if (NULL == buffer) {
		return NULL;
	}

	teal_csvR ret = tl_impl_calloc (1, sizeof(struct TL_CSV));

	ret->rows = tl_new_str_arr (buffer, "\n", &ret->n_rows);	// split csv by newlines
	ret->labels = tl_new_str_arr (ret->rows[0], ",", &ret->n_cols); // split first line by commas
	
	printf("n_rows in csv: %zu\n", ret->n_rows);

	char **shifted_rows = tl_impl_calloc (ret->n_rows - 1, sizeof(char *));

	for (size_t i = 1, j = 0; i < ret->n_rows; i++, j++) {
		shifted_rows[j] = ret->rows[i];
	}
	tl_impl_free (ret->rows[0]);
	tl_impl_free (ret->rows);
	ret->rows = shifted_rows;
	ret->n_rows--; // decrement after shifting rows

	ret->source_file = tl_str_dup (csv);
	ret->validated = false;

	tl_impl_free (buffer);
	return ret;
}

int 
tl_free_csv (teal_csvR *teal_csvRR) {

	for (size_t i = 0; i < (*teal_csvRR)->n_rows; i++) {
		tl_impl_free ((*teal_csvRR)->rows[i]);
	}
	for (size_t i = 0; i < (*teal_csvRR)->n_cols; i++) {
		tl_impl_free ((*teal_csvRR)->labels[i]);
	}
	tl_impl_free ((*teal_csvRR)->source_file);
	tl_impl_free ((*teal_csvRR)->rows);
	tl_impl_free ((*teal_csvRR)->labels);
	tl_impl_free (*teal_csvRR);
	*teal_csvRR = NULL;
	return 0;
}

size_t 
tl_csv_get_row_count (struct TL_CSV *teal_csvR) {

	return teal_csvR->n_rows;
}

size_t 
tl_csv_get_col_count (struct TL_CSV *teal_csvR) {

	return teal_csvR->n_cols;
}

char *
tl_csv_get_row_addr (struct TL_CSV *teal_csvR, size_t ind) {

	return teal_csvR->rows[ ind ];
}
