#include "felt_db.h"

#define FELT_STR_TYPE_MAX_LEN 				512


bool felt_row_input_valid_fns_set=false;
bool felt_write_row_field_fns_set=false;

char *TABLE_FIELD_TYPE_NAMES[] = 

			{	"STR",
				"ITR32",
				"ITR64",
				"DBL",
				"BLN",
				"DATE",
				"CURR",
				"CH",
				"ID",
				"REF",
				NULL };

const size_t TABLE_FIELD_TYPE_SIZES[] = 

			{	sizeof(char *), //STR
				sizeof(int32_t),
				sizeof(int64_t),
				sizeof(double) + sizeof(int16_t),
				sizeof(bool),
				sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t),
				sizeof(int64_t) + sizeof(uint8_t),
				sizeof(char), // CH
				sizeof(char *), 
				sizeof(struct Felt_Reference_Field)  };

felt_tabR
felt_new_table(char* label, size_t primary_index, char* schema) {
	felt_tabR table = felt_calloc(1, sizeof(struct Felt_Table));
	table->label = felt_str_dup(label);

	uuid_t bin_uuid;
	uuid_generate_random(bin_uuid);
	table->uuid = felt_calloc(UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper(bin_uuid, table->uuid);

	char **schema_inputs = felt_new_str_arr_split(schema, " ");

	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		table->n_cols++;
	}
	table->col_labels = felt_calloc(table->n_cols, sizeof(char *));
	
	table->schema = felt_calloc(table->n_cols, sizeof(TABLE_FIELD_TYPE));
	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		char *addr = NULL;
		if (!(addr = felt_find_str_in_str_arr(	TABLE_FIELD_TYPE_NAMES, schema_inputs[i]))) {
			goto abort;
		}
		size_t ind = (size_t)(addr - TABLE_FIELD_TYPE_NAMES[0]);
		table->schema[i] = ind;
		table->n_bytes_row += TABLE_FIELD_TYPE_SIZES[ind];
	}
	felt_free_split_str(schema_inputs);

	if (primary_index == 0) {
		table->primary_specified = false;
	} else {
		table->primary_specified = true;
		table->primary = primary_index;
	}
	return table;

abort:
	felt_free_split_str(schema_inputs);
	free(table->label);
	free(table->uuid);
	free(table->schema);
	free(table->col_labels);
	free(table);
	return NULL;
}

void
felt_free_table(felt_tabR *felt_tabRR) {
	free((*felt_tabRR)->label);
	free((*felt_tabRR)->uuid);
	free((*felt_tabRR)->schema);
	free((*felt_tabRR)->bytes);
	for (size_t i = 0; i < (*felt_tabRR)->n_cols; i++) {
		felt_free((*felt_tabRR)->col_labels[i]);
	}
	free((*felt_tabRR)->col_labels);
	free(*felt_tabRR);
	*felt_tabRR = NULL;
	return;
}
/*
int
felt_update_table_column_labelsV(int count, ...) {
	va_list args;
	va_start(args, count);
	felt_tabR table = va_arg(args, struct Felt_Table *);
	printf("here2\n");
	printf("%zu\n", table->n_cols);
	char *tmp = NULL;
	for (int i = 1; i < count + 1; i++) {
		printf("%d\n", i);
		if (!(tmp = felt_str_dup(va_arg(args, char *)))) {
			goto abort;
		}
		printf("%s\n", tmp);
		printf("here\n");
		table->col_labels[i - 1] = tmp;
		tmp = NULL;
	}
	return 0;

abort:
	for (int j = 0; j < count; j++) {
		felt_free(table->col_labels[j]);
	}
	return 1;
}
*/
int
felt_update_table_column_labels(felt_tabR table, char **labels) {
	for (size_t i = 0; i < table->n_cols; i++) {
		table->col_labels[i] = felt_str_dup(labels[i]);
	}
	return 0;
}

bool felt_row_input_valid_STR(char *value);
bool felt_row_input_valid_ITR32(char *value);
bool felt_row_input_valid_ITR64(char *value);
bool felt_row_input_valid_DBL(char *value);
bool felt_row_input_valid_BLN(char *value);
bool felt_row_input_valid_DATE(char *value);
bool felt_row_input_valid_CURR(char *value);
bool felt_row_input_valid_CH(char *value);
bool felt_row_input_valid_ID(char *value);
bool felt_row_input_valid_REF(char *value);

int felt_write_row_field_STR(char *value);
int felt_write_row_field_ITR32(char *value);
int felt_write_row_field_ITR64(char *value);
int felt_write_row_field_DBL(char *value);
int felt_write_row_field_BLN(char *value);
int felt_write_row_field_DATE(char *value);
int felt_write_row_field_CURR(char *value);
int felt_write_row_field_CH(char *value);
int felt_write_row_field_ID(char *value);
int felt_write_row_field_REF(char *value); //defined below

bool
felt_row_input_valid_STR(char* value) {
	if (felt_str_len(value) >= FELT_STR_TYPE_MAX_LEN) {
		return false;
	}
	return true;
}

bool 
felt_row_input_valid_ITR32(char *value) {
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
felt_row_input_valid_ITR64(char *value) {
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
felt_row_input_valid_DBL(char *value) {
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
felt_row_input_valid_BLN(char *value) {
	char *test = felt_str_dup(value);
	for (size_t i = 0; test[i] != '\0'; i++) {
		test[i] = tolower(test[i]);
	}
	if (felt_str_same(test, "true") || felt_str_same(test, "false")) {
		free(test);
		return true;
	}
	free(test);
	return false;
}

bool
felt_row_input_valid_DATE(char *value) {
	errno = 0;
	char **split = felt_new_str_arr_split(value, "-");
	size_t i = 0;
	bool ret = false;
	while (split[i] != NULL) {
		i++;
	}
	if (i < 3) {
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
	felt_free_split_str(split);
	return ret;
}

bool
felt_row_input_valid_CURR(char *value) {
	errno = 0;
	char **split = felt_new_str_arr_split(value, ".");
	size_t i = 0;
	bool ret = false;
	while (split[i] != NULL) {
		i++;
	}
	if (i != 2) {
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
	felt_free_split_str(split);
	return ret;
}

bool
felt_row_input_valid_CH(char* value) {
	if (felt_str_len(value) != 1) {
		return false;
	}
	return true;
}

bool (*felt_row_input_valid_fns[8]) (char *value);

int (*felt_write_row_field_fns[8]) (char *value);

int
felt_set_row_input_valid_fns(void) {
	felt_row_input_valid_fns[0] = felt_row_input_valid_STR;
	felt_row_input_valid_fns[1] = felt_row_input_valid_ITR32;
	felt_row_input_valid_fns[2] = felt_row_input_valid_ITR64;
	felt_row_input_valid_fns[3] = felt_row_input_valid_DBL;
	felt_row_input_valid_fns[4] = felt_row_input_valid_BLN;
	felt_row_input_valid_fns[5] = felt_row_input_valid_DATE;
	felt_row_input_valid_fns[6] = felt_row_input_valid_CURR;
	felt_row_input_valid_fns[7] = felt_row_input_valid_CH;
	/*
	felt_row_input_valid_fns[8] = felt_row_input_valid_ID;
	felt_row_input_valid_fns[9] = felt_row_input_valid_REF;
	*/
	felt_row_input_valid_fns_set = true;
	return 0;
}

int
felt_set_write_row_field_fns(void);

int
felt_insert_row_from_chars(felt_tabR table, char *row) {

	int ret = 0;

	if (!felt_row_input_valid_fns_set) {
		felt_set_row_input_valid_fns();
	}

	char **split_row = felt_new_str_arr_split(row, ",");

	size_t value_count = 0;
	while (split_row[value_count]) {
		value_count++;
	}
	if (value_count != table->n_cols) {
		ret = 1;
		goto cleanup;
	}

	for (size_t i = 0; split_row[i] != NULL; i++) {
		//printf("%zu: %s\n", i + 1, split_row[i]);
		bool result = false;
		if (!(result = (*felt_row_input_valid_fns[i])(split_row[i]))) {
			ret = 2;
			goto cleanup;
		}
	}
	/// will update to insert into data structure pending

cleanup:
	felt_free_split_str(split_row);
	return ret;
}
