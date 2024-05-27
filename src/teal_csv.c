#include "teal_csv.h"

teal_csvR 
teal_new_csv (char* csv) {

	char* buffer = teal_new_str_from_file (csv);
	if (NULL == buffer) {
		return NULL;
	}

	teal_csvR ret = impl_calloc (1, sizeof(struct Teal_CSV));

	ret->rows = teal_new_arr_from_str (buffer, "\n", &ret->n_rows);	// split csv by newlines
	ret->labels = teal_new_arr_from_str (ret->rows[0], ",", &ret->n_cols); // split first line by commas
	
	printf("n_rows in csv: %zu\n", ret->n_rows);

	char **shifted_rows = impl_calloc (ret->n_rows - 1, sizeof(char *));

	for (size_t i = 1, j = 0; i < ret->n_rows; i++, j++) {
		shifted_rows[j] = ret->rows[i];
	}
	impl_free (ret->rows[0]);
	impl_free (ret->rows);
	ret->rows = shifted_rows;
	ret->n_rows--; // decrement after shifting rows

	ret->source_file = teal_str_dup (csv);
	ret->validated = false;

	impl_free (buffer);
	return ret;
}

int 
teal_free_csv (teal_csvR *teal_csvRR) {

	for (size_t i = 0; i < (*teal_csvRR)->n_rows; i++) {
		impl_free ((*teal_csvRR)->rows[i]);
	}
	for (size_t i = 0; i < (*teal_csvRR)->n_cols; i++) {
		impl_free ((*teal_csvRR)->labels[i]);
	}
	impl_free ((*teal_csvRR)->source_file);
	impl_free ((*teal_csvRR)->rows);
	impl_free ((*teal_csvRR)->labels);
	impl_free (*teal_csvRR);
	*teal_csvRR = NULL;
	return 0;
}

void 
teal_debug_print_csv_info (teal_csvR csv) {


}