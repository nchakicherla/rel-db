#include "teal_db.h"

#define TEAL_STR_TYPE_MAX_LEN 				512
#define TEAL_DEFAULT_TAB_BYTES				512
#define NUM_IMPLEMENTED_TYPES				9

bool teal_field_input_valid_ptrs_set=false;
bool teal_write_field_input_ptrs_set=false;

char *TABLE_FIELD_TYPE_NAMES[] = 

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

const size_t TABLE_FIELD_TYPE_SIZES[] = 

			{	sizeof(char *), //STR
				sizeof(int32_t),
				sizeof(int64_t),
				sizeof(double) + sizeof(int16_t),
				sizeof(bool),
				sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t),
				sizeof(int64_t) + sizeof(uint8_t),
				sizeof(char), // CH
				// NOT YET IMPLEMENTED
				//
				//
				//
				sizeof(size_t),
				//
				//
				sizeof(teal_ref_field) };

void
teal_free_table(teal_tabR *teal_tabRR) {
	
	teal_free_table_data(*teal_tabRR); // relies on other struct members
	
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
teal_free_table_data(teal_tabR table) {
	/*
	size_t num_allocated_fields = 0;
	for (size_t i = 0; i < table->n_cols; i++) {
		if (table->schema[i] == STR) {
			num_allocated_fields++;
		}
	}
	size_t *free_locs = teal_calloc(num_allocated_fields, sizeof(size_t));

	for (size_t i = 0; i < num_allocated_fields; i++) {
		for (size_t j = 0; j < table->n_cols; j++) {

		}
	}
	*/

	size_t total_offset;
	for (size_t i = 0; i < table->n_rows; i++) {
		for (size_t j = 0; j < table->n_cols; j++) {
			if (table->schema[j] == STR) {
				printf("here2\n");
				total_offset = 	(i * table->n_bytes_row) + 
								TABLE_FIELD_TYPE_SIZES[ROW_ID] + 
								table->field_offsets[j];

				printf("%zu\n", TABLE_FIELD_TYPE_SIZES[ROW_ID]);
				printf("%zu\n", table->field_offsets[j]);
				printf("total off:%zu\n", total_offset);
				//*((char *)(table->bytes) + total_offset)
				//free(*(((char *)(table->bytes)) + total_offset));
				//void *source = (char *)(table->bytes) + total_offset;
				//printf("%s\n", (char*)source);
				//void  *new_ptr = source;
				//memcpy(new_ptr, source, sizeof(char *));
				/*
				void *source = (char *)(table->bytes) + total_offset;
				void **free_ref = teal_calloc(1, (sizeof(void *)));
				memcpy(free_ref, &source, TABLE_FIELD_TYPE_SIZES[STR]);
				free(*((char**)free_ref));
				free(free_ref);
				*/

				//char *str_ptr = (char *)(table->bytes) + total_offset;

				const void *ptr = (void *)((char *)(table->bytes) + total_offset);
				char *str_ptr = NULL;
				memcpy(&str_ptr, ptr, sizeof(char *));
				free(str_ptr);


				//free(new_ptr);
			}
		}
	}

	return;
}

int
teal_update_table_column_labels(teal_tabR table, char **labels) {
	for (size_t i = 0; i < table->n_cols; i++) {
		table->col_labels[i] = teal_str_dup(labels[i]);
	}
	return 0;
}
/*
bool teal_row_input_valid_STR(char *value);
bool teal_row_input_valid_ITR32(char *value);
bool teal_row_input_valid_ITR64(char *value);
bool teal_row_input_valid_DBL(char *value);
bool teal_row_input_valid_BLN(char *value);
bool teal_row_input_valid_DATE(char *value);
bool teal_row_input_valid_CURR(char *value);
bool teal_row_input_valid_CH(char *value);
bool teal_row_input_valid_ID(char *value);
bool teal_row_input_valid_REF(char *value);
*/
/*
int teal_write_row_field_STR(char *value);
int teal_write_row_field_ITR32(char *value);
int teal_write_row_field_ITR64(char *value);
int teal_write_row_field_DBL(char *value);
int teal_write_row_field_BLN(char *value);
int teal_write_row_field_DATE(char *value);
int teal_write_row_field_CURR(char *value);
int teal_write_row_field_CH(char *value);
int teal_write_row_field_ID(char *value);
int teal_write_row_field_REF(char *value);
*/ 
//defined below

bool
teal_row_input_valid_STR(char* value) {
	if (teal_str_len(value) >= TEAL_STR_TYPE_MAX_LEN) {
		return false;
	}
	return true;
}

bool 
teal_row_input_valid_ITR32(char *value) {
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
teal_row_input_valid_ITR64(char *value) {
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
teal_row_input_valid_DBL(char *value) {
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
teal_row_input_valid_BLN(char *value) {
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
teal_row_input_valid_DATE(char *value) {
	errno = 0;
	size_t count = 0;
	char **split = teal_new_str_arr_split(value, "-", &count);

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
	teal_free_split_str(split);
	return ret;
}

bool
teal_row_input_valid_CURR(char *value) {
	errno = 0;
	bool ret = false;
	size_t count = 0;
	char **split = teal_new_str_arr_split(value, ".", &count);

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
	teal_free_split_str(split);
	return ret;
}

bool
teal_row_input_valid_CH(char* value) {
	if (teal_str_len(value) != 1) {
		return false;
	}
	return true;
}

bool (*teal_field_input_valid_ptrs[NUM_IMPLEMENTED_TYPES]) (char *value);

int
teal_set_row_input_valid_fns(void) {
	teal_field_input_valid_ptrs[STR] = teal_row_input_valid_STR;
	teal_field_input_valid_ptrs[ITR32] = teal_row_input_valid_ITR32;
	teal_field_input_valid_ptrs[ITR64] = teal_row_input_valid_ITR64;
	teal_field_input_valid_ptrs[DBL] = teal_row_input_valid_DBL;
	teal_field_input_valid_ptrs[BLN] = teal_row_input_valid_BLN;
	teal_field_input_valid_ptrs[DATE] = teal_row_input_valid_DATE;
	teal_field_input_valid_ptrs[CURR] = teal_row_input_valid_CURR;
	teal_field_input_valid_ptrs[CH] = teal_row_input_valid_CH;
	/*
	teal_field_input_valid_ptrs[8] = teal_row_input_valid_REF;
	*/
	teal_field_input_valid_ptrs_set = true;
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
	//start_addr = (char *)alloc;

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
	char **split = teal_new_str_arr_split(value, "-", &count);

	uint16_t *uint16_cast = (uint16_t *) start_addr;
	*uint16_cast = (uint16_t) strtoul(split[2], NULL, 10); // year

	uint8_t *uint8_cast = (uint8_t *) start_addr;
	*(uint8_cast + 2) = strtoul(split[0], NULL, 10); // month
	*(uint8_cast + 3) = strtoul(split[1], NULL, 10); // day

	teal_free_split_str(split);
	return 0;
}

int 
teal_write_field_input_CURR(char *value, void *start_addr) {

	size_t count = 0;
	char **split = teal_new_str_arr_split(value, ".", &count);

	int64_t dollar_val = (int64_t) strtoll(split[0], NULL, 10);
	uint8_t cent_val = (uint8_t) strtoul(split[1], NULL, 10);

	teal_free_split_str(split);

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
int teal_write_field_input_ID(char *value, void *start_addr) {


return 0;
}

int teal_write_field_input_REF(char *value, void *start_addr);
*/
int (*teal_write_field_input_ptrs[NUM_IMPLEMENTED_TYPES])(char *value, void *start_addr);

int
teal_set_write_field_input_fns(void) {
	teal_write_field_input_ptrs[STR] = teal_write_field_input_STR;
	teal_write_field_input_ptrs[ITR32] = teal_write_field_input_ITR32;
	teal_write_field_input_ptrs[ITR64] = teal_write_field_input_ITR64;
	teal_write_field_input_ptrs[DBL] = teal_write_field_input_DBL;
	teal_write_field_input_ptrs[BLN] = teal_write_field_input_BLN;
	teal_write_field_input_ptrs[DATE] = teal_write_field_input_DATE;
	teal_write_field_input_ptrs[CURR] = teal_write_field_input_CURR;
	teal_write_field_input_ptrs[CH] = teal_write_field_input_CH;

	return 0;
}

teal_tabR
teal_new_table(char* label, size_t primary_index, char* schema) {

	if (!teal_field_input_valid_ptrs_set) {
		teal_set_row_input_valid_fns();
	}

	if (!teal_write_field_input_ptrs_set) {
		teal_set_write_field_input_fns();
	}

	teal_tabR table = teal_calloc(1, sizeof(struct Teal_Table));
	table->label = teal_str_dup(label);

	uuid_t bin_uuid;
	uuid_generate_random(bin_uuid);
	table->uuid = teal_calloc(UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper(bin_uuid, table->uuid);

	char **schema_inputs = teal_new_str_arr_split(schema, " ", &(table->n_cols));
	
	table->col_labels = teal_calloc(table->n_cols, sizeof(char *));
	table->schema = teal_calloc(table->n_cols, sizeof(TABLE_FIELD_TYPE));
	table->field_offsets = teal_calloc(table->n_cols, sizeof(size_t));

	for (size_t i = 0; i < table->n_cols; i++) {

		size_t type_index = teal_find_str_in_str_arr( TABLE_FIELD_TYPE_NAMES,
												schema_inputs[i]);

		if (type_index == ARR_INDEX_OOB) {
			goto cancel;
		}
		table->field_offsets[i] = table->n_bytes_row;
		printf("current offset: %zu\n", table->n_bytes_row);
		table->schema[i] = type_index;
		table->n_bytes_row += TABLE_FIELD_TYPE_SIZES[type_index];
	}
	teal_free_split_str(schema_inputs);

	table->n_bytes_schema = table->n_bytes_row;

	table->n_bytes_row += TABLE_FIELD_TYPE_SIZES[ROW_ID]; // add space for ROW_ID

	if (primary_index == 0) {
		table->primary_specified = false;
	} else {
		table->primary_specified = true;
		table->primary = primary_index;
	}
	return table;

cancel:
	teal_free_split_str(schema_inputs);
	free(table->label);
	free(table->uuid);
	free(table->schema);
	free(table->col_labels);
	free(table);
	return NULL;
}

int
teal_insert_row_from_chars(teal_tabR table, char *row) {

	int ret = 0;
	size_t value_count = 0;
	char **split_row = teal_new_str_arr_split(row, ",", &value_count);

	if (value_count != table->n_cols) {
		ret = 1;
		goto cleanup;
	}

	for (size_t i = 0; i < value_count; i++) {
		bool result = false;
		if (!(result = 
				(*teal_field_input_valid_ptrs[table->schema[i]])(split_row[i]))) {

			ret = 2;
			goto cleanup;
		}
	}
	if (!table->bytes) {
		table->bytes = teal_calloc(TEAL_DEFAULT_TAB_BYTES, sizeof(char));
		table->bytes_avail = TEAL_DEFAULT_TAB_BYTES;

	}
	while (table->bytes_avail < table->n_bytes_row) {
		teal_grow_table_bytes(table);
	}

	char *writer_addr = (char *) table->bytes;
	writer_addr += table->bytes_used;

	size_t *size_t_cast = (size_t *) writer_addr;
	*size_t_cast = table->n_rows + 1;
	
	table->n_rows++;

	writer_addr += TABLE_FIELD_TYPE_SIZES[ROW_ID];

	for (size_t i = 0; i < value_count; i++) {
	//printf("here2\n");
		(*teal_write_field_input_ptrs[table->schema[i]])(split_row[i], writer_addr);
		writer_addr += TABLE_FIELD_TYPE_SIZES[table->schema[i]];
	}

cleanup:
	teal_free_split_str(split_row);
	return ret;
}

int
teal_grow_table_bytes(teal_tabR table) {
	teal_grow_alloc(table->bytes, 2 * table->bytes_avail);
	table->bytes_avail *= 2;
	return 0;
}