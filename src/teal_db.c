#include "teal_db.h"

#define FELT_STR_TYPE_MAX_LEN 				512


bool teal_row_input_valid_fns_set=false;
bool teal_write_row_field_fns_set=false;

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
				sizeof(struct Teal_Reference_Field)  };

void
teal_free_table(teal_tabR *teal_tabRR) {
	free((*teal_tabRR)->label);
	free((*teal_tabRR)->uuid);
	free((*teal_tabRR)->schema);
	free((*teal_tabRR)->bytes);
	for (size_t i = 0; i < (*teal_tabRR)->n_cols; i++) {
		teal_free((*teal_tabRR)->col_labels[i]);
	}
	free((*teal_tabRR)->col_labels);
	free(*teal_tabRR);
	*teal_tabRR = NULL;
	return;
}
/*
int
teal_update_table_column_labelsV(int count, ...) {
	va_list args;
	va_start(args, count);
	teal_tabR table = va_arg(args, struct Teal_Table *);
	printf("here2\n");
	printf("%zu\n", table->n_cols);
	char *tmp = NULL;
	for (int i = 1; i < count + 1; i++) {
		printf("%d\n", i);
		if (!(tmp = teal_str_dup(va_arg(args, char *)))) {
			goto cancel;
		}
		printf("%s\n", tmp);
		printf("here\n");
		table->col_labels[i - 1] = tmp;
		tmp = NULL;
	}
	return 0;

cancel:
	for (int j = 0; j < count; j++) {
		teal_free(table->col_labels[j]);
	}
	return 1;
}
*/
int
teal_update_table_column_labels(teal_tabR table, char **labels) {
	for (size_t i = 0; i < table->n_cols; i++) {
		table->col_labels[i] = teal_str_dup(labels[i]);
	}
	return 0;
}

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

int teal_write_row_field_STR(char *value);
int teal_write_row_field_ITR32(char *value);
int teal_write_row_field_ITR64(char *value);
int teal_write_row_field_DBL(char *value);
int teal_write_row_field_BLN(char *value);
int teal_write_row_field_DATE(char *value);
int teal_write_row_field_CURR(char *value);
int teal_write_row_field_CH(char *value);
int teal_write_row_field_ID(char *value);
int teal_write_row_field_REF(char *value); //defined below

bool
teal_row_input_valid_STR(char* value) {
	if (teal_str_len(value) >= FELT_STR_TYPE_MAX_LEN) {
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
		test[i] = tolower(test[i]);
	}
	if (teal_str_same(test, "true") || teal_str_same(test, "false")) {
		free(test);
		return true;
	}
	free(test);
	return false;
}

bool
teal_row_input_valid_DATE(char *value) {
	errno = 0;
	char **split = teal_new_str_arr_split(value, "-");
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
	teal_free_split_str(split);
	return ret;
}

bool
teal_row_input_valid_CURR(char *value) {
	errno = 0;
	char **split = teal_new_str_arr_split(value, ".");
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

bool (*teal_row_input_valid_fns[8]) (char *value);

int (*teal_write_row_field_fns[8]) (char *value);

int
teal_set_row_input_valid_fns(void) {
	teal_row_input_valid_fns[0] = teal_row_input_valid_STR;
	teal_row_input_valid_fns[1] = teal_row_input_valid_ITR32;
	teal_row_input_valid_fns[2] = teal_row_input_valid_ITR64;
	teal_row_input_valid_fns[3] = teal_row_input_valid_DBL;
	teal_row_input_valid_fns[4] = teal_row_input_valid_BLN;
	teal_row_input_valid_fns[5] = teal_row_input_valid_DATE;
	teal_row_input_valid_fns[6] = teal_row_input_valid_CURR;
	teal_row_input_valid_fns[7] = teal_row_input_valid_CH;
	/*
	teal_row_input_valid_fns[8] = teal_row_input_valid_ID;
	teal_row_input_valid_fns[9] = teal_row_input_valid_REF;
	*/
	teal_row_input_valid_fns_set = true;
	return 0;
}

int
teal_set_write_row_field_fns(void);

int
teal_insert_row_from_chars(teal_tabR table, char *row) {

	int ret = 0;
	char **split_row = teal_new_str_arr_split(row, ",");
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
		if (!(result = (*teal_row_input_valid_fns[i])(split_row[i]))) {
			ret = 2;
			goto cleanup;
		}
	}
	/// will update to insert into data structure pending

cleanup:
	teal_free_split_str(split_row);
	return ret;
}

teal_tabR
teal_new_table(char* label, size_t primary_index, char* schema) {

	if (!teal_row_input_valid_fns_set) {
		teal_set_row_input_valid_fns();
	}

	teal_tabR table = teal_calloc(1, sizeof(struct Teal_Table));
	table->label = teal_str_dup(label);

	uuid_t bin_uuid;
	uuid_generate_random(bin_uuid);
	table->uuid = teal_calloc(UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper(bin_uuid, table->uuid);

	char **schema_inputs = teal_new_str_arr_split(schema, " ");

	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		table->n_cols++;
	}
	table->col_labels = teal_calloc(table->n_cols, sizeof(char *));
	
	table->schema = teal_calloc(table->n_cols, sizeof(TABLE_FIELD_TYPE));
	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		char *addr = NULL;
		if (!(addr = teal_find_str_in_str_arr(TABLE_FIELD_TYPE_NAMES, schema_inputs[i]))) {
			goto cancel;
		}
		size_t ind = (size_t)(addr - TABLE_FIELD_TYPE_NAMES[0]);
		table->schema[i] = ind;
		table->n_bytes_row += TABLE_FIELD_TYPE_SIZES[ind];
	}
	teal_free_split_str(schema_inputs);

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