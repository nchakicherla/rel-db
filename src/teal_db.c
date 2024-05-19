#include "teal_db.h"

#define TEAL_STR_TYPE_MAX_LEN 				512
#define TEAL_TABLE_DEFAULT_BYTES			512
#define NUM_IMPLEMENTED_TYPES				9

bool teal_input_validate_fns_set=false;
bool teal_field_writer_fns_set=false;

char *TEAL_FIELD_TYPE_NAMES[] = 

			{	"STR",
				"ITR32",
				"ITR64",
				"DBL",
				"BLN",
				"DATE",
				"CURR",
				"CH",
				"ROW_ID",
				"REF",
				NULL };

const size_t TEAL_FIELD_TYPE_SIZES[] = 

			{	sizeof(char *),
				sizeof(int32_t),
				sizeof(int64_t),
				sizeof(double) + sizeof(int16_t),
				sizeof(bool),
				sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t),
				sizeof(int64_t) + sizeof(uint8_t),
				sizeof(char),
				sizeof(size_t),
				// pending implement
				sizeof(teal_field_reference) };

void
teal_table_free(teal_tabR *teal_tabRR) {
	
	teal_table_free_bytes(*teal_tabRR); // frees any allocations
	free((*teal_tabRR)->label);
	free((*teal_tabRR)->uuid);
	free((*teal_tabRR)->schema);
	free((*teal_tabRR)->field_offsets);
	for (size_t i = 0; i < (*teal_tabRR)->n_cols; i++) {
		teal_free((*teal_tabRR)->col_labels[i]);
	}
	free((*teal_tabRR)->col_labels);
	free((*teal_tabRR)->bytes);
	free(*teal_tabRR);
	*teal_tabRR = NULL;
	return;
}

void
teal_table_free_bytes(teal_tabR table) {

	size_t total_offset;
	for (size_t i = 0; i < table->n_rows; i++) {
		for (size_t j = 0; j < table->n_cols; j++) {
			if (table->schema[j] == STR) {
										// start at & current row
				total_offset = 	(i * table->n_bytes_row) 
										// account for size_t attached to each row
								+ TEAL_FIELD_TYPE_SIZES[ROW_ID]
										// account for any prior fields
								+ table->field_offsets[j];
				/*
				const void *ptr = ((char *)(table->bytes) + total_offset);
				char *str_ptr = NULL;
				memcpy(&str_ptr, ptr, TEAL_FIELD_TYPE_SIZES[STR]);
				//free(str_ptr);
				*/
				free(*(char**)((char *)(table->bytes) + total_offset));
			}
		}
	}
	return;
}

int
teal_table_update_labels(teal_tabR table, char **labels) {

	for (size_t i = 0; i < table->n_cols; i++) {
		table->col_labels[i] = teal_str_dup(labels[i]);
	}
	return 0;
}

bool
teal_validate_input_STR(char* value) {

	if (teal_str_len(value) >= TEAL_STR_TYPE_MAX_LEN) {
		return false;
	}
	return true;
}

bool 
teal_validate_input_ITR32(char *value) {

	errno = 0;
	long test = 0;
	char* ptr;

	test = strtol(value, &ptr, 10);

	if (ptr == value || *ptr != '\0' ||
		((test == LONG_MIN || test == LONG_MAX) && errno == ERANGE)) {

		return false;
	}
	return true;
}

bool 
teal_validate_input_ITR64(char *value) {

	errno = 0;
	long long test = 0;
	char* ptr;

	test = strtoll(value, &ptr, 10);

	if (ptr == value || *ptr != '\0' 
		|| ((test == LLONG_MIN || test == LLONG_MAX) && errno == ERANGE)) {

		return false;
	}
	return true;
}

bool
teal_validate_input_DBL(char *value) {

	errno = 0;
	double test = 0;
	char* ptr = value;

	test = strtod(value, &ptr);

	if (ptr == value || *ptr != '\0'
		|| (test == HUGE_VAL && errno == ERANGE)) {

		return false;
	}
	return true;
}

bool
teal_validate_input_BLN(char *value) {

	char *test = teal_str_dup(value);
	for (size_t i = 0; test[i] != '\0'; i++) {
		test[i] = toupper(test[i]);
	}
	if (teal_str_same(test, "TRUE") || teal_str_same(test, "TRUE")) {
		free(test);
		return true;
	}
	free(test);
	return false;
}

bool
teal_validate_input_DATE(char *value) {

	errno = 0;
	size_t count = 0;
	char **split = teal_new_arr_from_str(value, "-", &count);

	bool ret = false;
	if (count < 3) {
		goto cleanup;
	}

	char *ptr = split[0];
	long numeric = strtol(split[0], &ptr, 10); // date
	if (errno == ERANGE || *ptr != '\0') goto cleanup;
	else if (numeric < 1 || numeric > 12) goto cleanup;
	
	ptr = split[1];
	numeric = strtol(split[1], &ptr, 10);
	if (errno == ERANGE || *ptr != '\0') goto cleanup;
	else if (numeric < 1 || numeric > 31) goto cleanup;

	ptr = split[2];
	numeric = strtol(split[2], &ptr, 10);
	if (errno == ERANGE || *ptr != '\0') goto cleanup;
	else if (numeric < 1700 || numeric > 9999) goto cleanup;

	ret = true;

cleanup:
	teal_free_str_arr(split);
	return ret;
}

bool
teal_validate_input_CURR(char *value) {

	errno = 0;
	bool ret = false;
	size_t count = 0;
	char **split = teal_new_arr_from_str(value, ".", &count);

	if (count != 2) {
		goto cleanup;
	}

	char *ptr = split[0];
	long long test = 0;

	test = strtoll(split[0], &ptr, 10);

	if (ptr == split[0] || *ptr != '\0' ||
		((test == LLONG_MIN || test == LLONG_MAX) && errno == ERANGE)) {
		
		goto cleanup;
	}

	ptr = split[1];
	test = 0;

	test = strtoll(split[1], &ptr, 10);

	if (ptr == split[1] || *ptr != '\0' ||
		((test == LLONG_MIN || test == LLONG_MAX) && errno == ERANGE)) {

		goto cleanup;
	} else if (test > 99 || test < 0) {
		goto cleanup;
	}

	ret = true;

cleanup:
	teal_free_str_arr(split);
	return ret;
}

bool
teal_validate_input_CH(char* value) {

	if (teal_str_len(value) != 1) {
		return false;
	}
	return true;
}

bool (*teal_is_input_valid[NUM_IMPLEMENTED_TYPES]) (char *value);

int
teal_set_validation_fns(void) {

	teal_is_input_valid[STR] = teal_validate_input_STR;
	teal_is_input_valid[ITR32] = teal_validate_input_ITR32;
	teal_is_input_valid[ITR64] = teal_validate_input_ITR64;
	teal_is_input_valid[DBL] = teal_validate_input_DBL;
	teal_is_input_valid[BLN] = teal_validate_input_BLN;
	teal_is_input_valid[DATE] = teal_validate_input_DATE;
	teal_is_input_valid[CURR] = teal_validate_input_CURR;
	teal_is_input_valid[CH] = teal_validate_input_CH;
	/*
	teal_is_input_valid[8] = teal_row_input_valid_REF;
	*/
	teal_input_validate_fns_set = true;
	return 0;
}

int 
teal_write_field_input_STR(char *value, void *start_addr) {
	size_t len = teal_str_len(value);
	char* alloc = teal_calloc(len + 1, sizeof(char));
	for (size_t i = 0; i < len; i++) {
		alloc[i] = value[i];
	}
	alloc[len] = '\0';

	memcpy(start_addr, &alloc, sizeof(char *));
	return 0;
}

int 
teal_write_field_input_ITR32(char *value, void *start_addr) {

	long int_val = strtol(value, NULL, 10);
	int32_t *int32_cast = (int32_t *) start_addr;
	*int32_cast = (int32_t) int_val;

	return 0; 
}

int 
teal_write_field_input_ITR64(char *value, void *start_addr) {

	long long int_val = strtoll(value, NULL, 10);
	int64_t *int64_cast = (int64_t *) start_addr;
	*int64_cast = (int64_t) int_val;

	return 0;
}

int 
teal_write_field_input_DBL(char *value, void *start_addr) {

	double dbl_value = strtod(value, NULL);
	double *double_cast = (double *) start_addr;
	*double_cast = dbl_value;

	return 0;
}

int 
teal_write_field_input_BLN(char *value, void *start_addr) {

	char *alloc = teal_str_dup(value);
	bool *bool_cast = (bool *) start_addr;
	for (size_t i = 0; alloc[i] != '\0'; i++) {
		alloc[i] = toupper(alloc[i]);
	}
	if (teal_str_same(alloc, "TRUE")) {
		*bool_cast = true;
	} else {
		*bool_cast = false;
	}
	free(alloc);
	return 0;

}
int 
teal_write_field_input_DATE(char *value, void *start_addr) {

	size_t count = 0;
	char **split = teal_new_arr_from_str(value, "-", &count);

	uint16_t *uint16_cast = (uint16_t *) start_addr;
	*uint16_cast = (uint16_t) strtoul(split[2], NULL, 10); // year

	uint8_t *uint8_cast = (uint8_t *) start_addr;
	*(uint8_cast + 2) = strtoul(split[0], NULL, 10); // month
	*(uint8_cast + 3) = strtoul(split[1], NULL, 10); // day

	teal_free_str_arr(split);
	return 0;
}

int 
teal_write_field_input_CURR(char *value, void *start_addr) {

	size_t count = 0;
	char **split = teal_new_arr_from_str(value, ".", &count);

	int64_t dollar_val = (int64_t) strtoll(split[0], NULL, 10);
	uint8_t cent_val = (uint8_t) strtoul(split[1], NULL, 10);

	teal_free_str_arr(split);

	int64_t *int64_cast = (int64_t *) start_addr;
	*int64_cast = dollar_val;

	uint8_t *uint8_cast = (uint8_t *) start_addr;
	*(uint8_cast + 4) = cent_val;

	return 0;
}

int 
teal_write_field_input_CH(char *value, void *start_addr) {

	char *char_cast = (char *) start_addr;
	*char_cast = *value;
	return 0;

}
/*
int teal_write_field_input_REF(char *value, void *start_addr);
*/
int (*teal_field_write_ptrs[NUM_IMPLEMENTED_TYPES])(char *value, void *start_addr);

int
teal_set_write_field_input_fns(void) {
	teal_field_write_ptrs[STR] = teal_write_field_input_STR;
	teal_field_write_ptrs[ITR32] = teal_write_field_input_ITR32;
	teal_field_write_ptrs[ITR64] = teal_write_field_input_ITR64;
	teal_field_write_ptrs[DBL] = teal_write_field_input_DBL;
	teal_field_write_ptrs[BLN] = teal_write_field_input_BLN;
	teal_field_write_ptrs[DATE] = teal_write_field_input_DATE;
	teal_field_write_ptrs[CURR] = teal_write_field_input_CURR;
	teal_field_write_ptrs[CH] = teal_write_field_input_CH;
	return 0;
}

teal_tabR
teal_new_table(char* label, size_t primary_index, char* schema) {

	if (!teal_input_validate_fns_set) {
		teal_set_validation_fns();
	}
	if (!teal_field_writer_fns_set) {
		teal_set_write_field_input_fns();
	}

	teal_tabR table = teal_calloc(1, sizeof(struct Teal_Table));
	table->label = teal_str_dup(label);

	uuid_t bin_uuid;
	uuid_generate_random(bin_uuid);
	table->uuid = teal_calloc(UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper(bin_uuid, table->uuid);

	char **schema_inputs = teal_new_arr_from_str(schema, " ", &(table->n_cols));
	
	table->col_labels = teal_calloc(table->n_cols, sizeof(char *));
	table->schema = teal_calloc(table->n_cols, sizeof(TABLE_FIELD_TYPE));
	table->field_offsets = teal_calloc(table->n_cols, sizeof(size_t));

	for (size_t i = 0; i < table->n_cols; i++) {

		size_t type_index = teal_scan_arr_for_str( TEAL_FIELD_TYPE_NAMES,
												schema_inputs[i]);

		if (type_index == ARR_INDEX_OOB) {
			goto cancel;
		}
		table->field_offsets[i] = table->n_bytes_row;
		table->schema[i] = type_index;
		table->n_bytes_row += TEAL_FIELD_TYPE_SIZES[type_index];
	}
	teal_free_str_arr(schema_inputs);

	table->n_bytes_schema = table->n_bytes_row;
	table->n_bytes_row += TEAL_FIELD_TYPE_SIZES[ROW_ID]; // add space for ROW_ID

	if (primary_index == 0) {
		table->has_primary = false;
	} else {
		table->has_primary = true;
		table->primary = primary_index;
	}
	return table;

cancel:
	teal_free_str_arr(schema_inputs);
	free(table->label);
	free(table->uuid);
	free(table->schema);
	free(table->col_labels);
	free(table);
	return NULL;
}

int
teal_table_insert_row(teal_tabR table, char *row) {

	int ret = 0;
	size_t value_count = 0;
	char **split_row = teal_new_arr_from_str(row, ",", &value_count);

	if (value_count != table->n_cols) {
		ret = 1;
		goto cleanup;
	}

	for (size_t i = 0; i < value_count; i++) {
		bool result = false;
		if (!(result = 
				(*teal_is_input_valid[table->schema[i]])(split_row[i]))) {
			ret = 2;
			goto cleanup;
		}
	}
	if (!table->bytes) {
		table->bytes = teal_calloc(TEAL_TABLE_DEFAULT_BYTES, sizeof(char));
		table->bytes_avail = TEAL_TABLE_DEFAULT_BYTES;

	}
	while (table->bytes_avail < table->n_bytes_row) {
		teal_table_grow_bytes_2x(table);
	}

	char *writer_addr = (char *) table->bytes;
	writer_addr += table->bytes_used;

	size_t *size_t_cast = (size_t *) writer_addr;
	*size_t_cast = table->n_rows + 1; // write ROW_ID
	
	table->n_rows++;

	writer_addr += TEAL_FIELD_TYPE_SIZES[ROW_ID];

	for (size_t i = 0; i < value_count; i++) {
		(*teal_field_write_ptrs[table->schema[i]])(split_row[i], writer_addr);
		writer_addr += TEAL_FIELD_TYPE_SIZES[table->schema[i]];
	}

	table->bytes_used += table->n_bytes_row;

cleanup:
	teal_free_str_arr(split_row);
	return ret;
}

int
teal_table_grow_bytes_2x(teal_tabR table) {

	teal_grow_alloc(table->bytes, 2 * table->bytes_avail);
	table->bytes_avail *= 2;
	return 0;
}


void
teal_debug_print_table_info(teal_tabR table) {

	printf("label: %s\n", table->label);
	printf("uuid: %s\n", table->uuid);
	printf("n_bytes_schema: %zu\n", table->n_bytes_schema);
	printf("n_bytes_row: %zu\n", table->n_bytes_row);
	printf("ncols: %zu, nrows: %zu\n", table->n_cols, table->n_rows);
	printf("bytes_avail: %zu\n", table->bytes_avail);
	printf("bytes_used: %zu\n", table->bytes_used);
	return;
}
