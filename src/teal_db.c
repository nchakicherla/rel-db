#include "teal_db.h"

#define META_NUM_TYPES_IMPLEMENTED 8


#define DATA_BUFFER_DEFAULT_SIZE 512
#define DATA_BUFFER_GROWTH_FACTOR 2.0f

#define TEAL_STR_TYPE_MAX_LEN 8192

#define MARA_FLT_TYPE_MAX_VAL 1e20
#define MARA_FLT_TYPE_MAX_FRAC_DIGS 30
#define MARA_FLT_TYPE_MAX_SIGFIGS 15

bool teal_validate_input_fnptrs_set = false;
bool teal_write_field_fnptrs_set = false;
bool teal_print_field_value_fnptrs_set = false;

char *TEAL_FIELD_TYPE_NAMES[] = 
{
	"STR",
	"ITR32",
	"ITR64",
	"FLT",
	"BLN",
	"DATE",
	"CURR",
	"CH",
	"ROW_ID",
	"REF",
	NULL
};

const size_t TEAL_FIELD_TYPE_SIZES[] = 
{
	sizeof(char *),
	sizeof(int32_t),
	sizeof(int64_t),
	sizeof(double) + sizeof(int),
	sizeof(bool),
	sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t),
	sizeof(int64_t) + sizeof(uint8_t),
	sizeof(char),
	sizeof(size_t),
	// pending implement
	sizeof(teal_field_reference)
};

typedef struct Mara_Parsed_Float { // taken from my mara library
	double val;
	int frac_digs;
} *mara_fltR;

mara_fltR 
__mara_new_parsed_float (char *str) {

	size_t len = teal_str_len(str);

	for (size_t i = 0; i < len; i++) {
		if (!isdigit(str[i]) && str[i] != '.' && str[i] != 'e' && str[i] != '-') {
			return NULL;
		}
	}

	char *end_ptr = str;
	double val = strtod(str, &end_ptr);
	int frac_digs = 0;

	if (end_ptr == str) {
		return NULL;
	}

	char *e_loc = teal_str_chr(str, 'e', len);
	char *dec_loc = teal_str_chr(str, '.', len);

    if (dec_loc) {
        if(*(dec_loc + 1) != '\0') { // check for second decimal
            if(teal_str_chr(dec_loc + 1, '.', len - (size_t)(dec_loc - str + 1))) {
                return NULL;
            }
        }
    }
    if(e_loc) {
        if(*(e_loc + 1) != '\0') {
            if(teal_str_chr(e_loc + 1, 'e', len - (size_t)(e_loc - str + 1))) {
                return NULL;
            }            
        }
    }

    if(e_loc && *(e_loc + 1) == '-') {           
        char* it = e_loc;
        if(dec_loc) {
            while(*(it - 1) != '.') {
                it--;
                frac_digs++;
            }
        }
        frac_digs += (int)strtol(e_loc + 2, NULL, 10);
    } else if (!e_loc && dec_loc) {
        frac_digs += (len - (int)(dec_loc - str + 1));
    }

    if(frac_digs > MARA_FLT_TYPE_MAX_FRAC_DIGS) {
        return NULL;
    }
    if(e_loc != NULL && (size_t)(e_loc - str) - 1 > MARA_FLT_TYPE_MAX_SIGFIGS) {
        return NULL;
    }
    if(val > MARA_FLT_TYPE_MAX_VAL) {
        return NULL;
    }

    mara_fltR parsed_flt = __teal_calloc(1, sizeof(struct Mara_Parsed_Float));

    parsed_flt->val = val;
    parsed_flt->frac_digs = frac_digs;

    return parsed_flt;
}

void 
teal_print_field_value_STR (void *addr) {

	printf ("%s", *(char **) addr);
	return;
}

void 
teal_print_field_value_ITR32 (void *addr) {

	printf ("%" PRId32 "", *(int32_t *) addr);
	return;
}

void 
teal_print_field_value_ITR64 (void *addr) {

	printf ("%" PRId64 "", *(int64_t *) addr);
	return;
}

void 
teal_print_field_value_DBL (void *addr) {

	double val = *(double *) addr;
	int frac_digs = *(int *)((char *) addr + sizeof(double));

	printf ("%.*f", frac_digs, val);
	return;
}

void 
teal_print_field_value_BLN (void *addr) {

	if (true == *(bool *) addr) {
		printf ("TRUE");
	} else {
		printf ("FALSE");
	}
	return;
}

void 
teal_print_field_value_DATE (void *addr) {

	char* ptr = (char *) addr;
	printf ("%" PRIu16 "/", *(uint16_t *) ptr);
	printf ("%" PRIu8 "/", *(uint8_t *)(ptr + sizeof(uint16_t)));
	printf ("%" PRIu8 "", *(uint8_t *)(ptr + sizeof(uint16_t) + sizeof(uint8_t)));
	return;
}

void 
teal_print_field_value_CURR (void *addr) {

	char *ptr = (char *) addr;
	printf ("$%" PRId64 ".", *(int64_t *)ptr);
	printf ("%" PRIu8 "", *(uint8_t *)(ptr + sizeof(int64_t)));
	return;
}

void 
teal_print_field_value_CH (void *addr) {

	char *ptr = (char *) addr;
	printf ("%c", *ptr);
	return;
}

void ( *teal_print_field_value_fnptrs[ META_NUM_TYPES_IMPLEMENTED ]) (void *addr);

int 
teal_set_print_field_value_fnptrs (void) {

	teal_print_field_value_fnptrs[STR] = teal_print_field_value_STR;
	teal_print_field_value_fnptrs[ITR32] = teal_print_field_value_ITR32;
	teal_print_field_value_fnptrs[ITR64] = teal_print_field_value_ITR64;
	teal_print_field_value_fnptrs[FLT] = teal_print_field_value_DBL;
	teal_print_field_value_fnptrs[BLN] = teal_print_field_value_BLN;
	teal_print_field_value_fnptrs[DATE] = teal_print_field_value_DATE;
	teal_print_field_value_fnptrs[CURR] = teal_print_field_value_CURR;
	teal_print_field_value_fnptrs[CH] = teal_print_field_value_CH;

	teal_print_field_value_fnptrs_set = true;
	return 0;
}

void *
teal_get_row_addr (teal_tabR table, size_t ind) {

	void *addr = (char *) table->bytes + ((ind - 1) * table->n_bytes_row);
	return addr;
}

void 
teal_print_row (teal_tabR table, void *addr) {

	char *ptr = addr;
	printf ("%zu\t",  *(size_t *) ptr);
	ptr += TEAL_FIELD_TYPE_SIZES[ ROW_ID ];

	for ( size_t i = 0; i < table->n_cols; i++ ) {

		( *teal_print_field_value_fnptrs[ table->schema[i] ])( (char *) ptr + table->field_offsets[i] );

		if (i < table->n_cols - 1) {
			printf("\t");
		} else {
			printf("\n");
		}
	}
	return;
}

void 
teal_table_free (teal_tabR *teal_tabRR) {
	
	size_t total_offset; // free any STR allocations
	for (size_t i = 0; i < (*teal_tabRR)->n_rows; i++) {
		for (size_t j = 0; j < (*teal_tabRR)->n_cols; j++) {

			if ((*teal_tabRR)->schema[j] == STR) {
				// total_offset = prior row offset + row_id size + prior fields in row offset
				total_offset = 	(i * (*teal_tabRR)->n_bytes_row) + TEAL_FIELD_TYPE_SIZES[ROW_ID] + (*teal_tabRR)->field_offsets[j];
				__teal_free ( *(char**)( (char *)((*teal_tabRR)->bytes) + total_offset));
			}
		}
	}
	__teal_free ((*teal_tabRR)->label);
	__teal_free ((*teal_tabRR)->uuid);
	__teal_free ((*teal_tabRR)->schema);
	__teal_free ((*teal_tabRR)->field_offsets);
	for (size_t i = 0; i < (*teal_tabRR)->n_cols; i++) {
		__teal_free ((*teal_tabRR)->col_labels[i]);
	}
	__teal_free ((*teal_tabRR)->col_labels);
	__teal_free ((*teal_tabRR)->bytes);
	__teal_free (*teal_tabRR);
	*teal_tabRR = NULL;
	return;
}

int 
teal_table_update_labels (teal_tabR table, char **labels) {

	for (size_t i = 0; i < table->n_cols; i++) {
		if (table->col_labels[i]) {
			__teal_free (table->col_labels[i]);
		}
		table->col_labels[i] = teal_str_dup (labels[i]);
	}
	return 0;
}

bool 
teal_validate_input_STR (char* value) {

	if (teal_str_len (value) >= TEAL_STR_TYPE_MAX_LEN) {
		return false;
	}
	return true;
}

bool 
teal_validate_input_ITR32 (char *value) {

	errno = 0;
	long test = 0;
	char* ptr;

	test = strtol (value, &ptr, 10);

	if (ptr == value || *ptr != '\0' ||
		((test == LONG_MIN || test == LONG_MAX) && errno == ERANGE)) {

		return false;
	}
	return true;
}

bool 
teal_validate_input_ITR64 (char *value) {

	errno = 0;
	long long test = 0;
	char* ptr;

	test = strtoll (value, &ptr, 10);

	if (ptr == value || *ptr != '\0' 
		|| ((test == LLONG_MIN || test == LLONG_MAX) && errno == ERANGE)) {

		return false;
	}
	return true;
}

bool 
teal_validate_input_DBL (char *value) {
/*
	errno = 0;
	double test = 0;
	char* ptr = value;

	test = strtod (value, &ptr);

	if (ptr == value || *ptr != '\0'
		|| (test == HUGE_VAL && errno == ERANGE)) {

		return false;
	}
	return true;
*/
	mara_fltR parsed_flt = __mara_new_parsed_float (value);
	if (NULL == parsed_flt) {
		return false;
	}
	__teal_free (parsed_flt);
	return true;
}

bool 
teal_validate_input_BLN (char *value) {

	char *test = teal_str_dup (value);

	for (size_t i = 0; test[i] != '\0'; i++) {
		test[i] = toupper (test[i]);
	}

	if ( teal_str_same (test, "TRUE") || teal_str_same (test, "FALSE") ) {
		__teal_free (test);
		return true;
	}

	__teal_free (test);
	return false;
}

bool 
teal_validate_input_DATE (char *value) {

	errno = 0;
	size_t count = 0;
	char **split = teal_new_arr_from_str (value, "-", &count);

	bool ret = false;
	if (count < 3) {
		goto cleanup;
	}

	char *ptr = split[0];
	long numeric = strtol (split[0], &ptr, 10); // date

	if (errno == ERANGE || *ptr != '\0') goto cleanup;
	else if (numeric < 1 || numeric > 12) goto cleanup; // month
	
	ptr = split[1];
	numeric = strtol (split[1], &ptr, 10);
	if (errno == ERANGE || *ptr != '\0') goto cleanup;
	else if (numeric < 1 || numeric > 31) goto cleanup; // day

	ptr = split[2];
	numeric = strtol (split[2], &ptr, 10);
	if (errno == ERANGE || *ptr != '\0') goto cleanup;
	else if (numeric < 1700 || numeric > 9999) goto cleanup; // year

	ret = true;

cleanup:
	teal_free_str_arr (split);
	return ret;
}

bool
teal_validate_input_CURR (char *value) {

	errno = 0;
	bool ret = false;
	size_t count = 0;
	char **split = teal_new_arr_from_str (value, ".", &count);

	if (count != 2) {
		goto cleanup;
	}

	char *ptr = split[0];
	long long test = 0;

	test = strtoll (split[0], &ptr, 10);

	if (ptr == split[0] || *ptr != '\0' ||
		((test == LLONG_MIN || test == LLONG_MAX) && errno == ERANGE)) {
		
		goto cleanup;
	}

	ptr = split[1];
	test = 0;

	test = strtoll (split[1], &ptr, 10);

	if (ptr == split[1] || *ptr != '\0' ||
		((test == LLONG_MIN || test == LLONG_MAX) && errno == ERANGE)) {

		goto cleanup;
	} else if (test > 99 || test < 0) {
		goto cleanup;
	}

	ret = true;

cleanup:
	teal_free_str_arr (split);
	return ret;
}

bool
teal_validate_input_CH (char* value) {

	if (teal_str_len (value) != 1) {
		return false;
	}
	return true;
}

bool ( *teal_validate_input_fnptrs[ META_NUM_TYPES_IMPLEMENTED ]) (char *value);

int
teal_set_validate_input_fnptrs (void) {

	teal_validate_input_fnptrs[STR] = teal_validate_input_STR;
	teal_validate_input_fnptrs[ITR32] = teal_validate_input_ITR32;
	teal_validate_input_fnptrs[ITR64] = teal_validate_input_ITR64;
	teal_validate_input_fnptrs[FLT] = teal_validate_input_DBL;
	teal_validate_input_fnptrs[BLN] = teal_validate_input_BLN;
	teal_validate_input_fnptrs[DATE] = teal_validate_input_DATE;
	teal_validate_input_fnptrs[CURR] = teal_validate_input_CURR;
	teal_validate_input_fnptrs[CH] = teal_validate_input_CH;
	/*
	teal_validate_input_fnptrs[8] = teal_row_input_valid_REF;
	*/
	teal_validate_input_fnptrs_set = true;
	return 0;
}

int 
teal_write_field_input_STR (char *value, void *start_addr) {

	size_t len = teal_str_len (value);
	char* alloc = __teal_calloc (len + 1, sizeof(char));

	for (size_t i = 0; i < len; i++) {
		alloc[i] = value[i];
	}
	alloc[len] = '\0';

	memcpy (start_addr, &alloc, sizeof(char *));
	return 0;
}

int 
teal_write_field_input_ITR32 (char *value, void *start_addr) {

	long int_val = strtol (value, NULL, 10);
	int32_t *int32_cast = (int32_t *) start_addr;
	*int32_cast = (int32_t) int_val;

	return 0; 
}

int 
teal_write_field_input_ITR64 (char *value, void *start_addr) {

	long long int_val = strtoll (value, NULL, 10);
	int64_t *int64_cast = (int64_t *) start_addr;
	*int64_cast = (int64_t) int_val;

	return 0;
}

int 
teal_write_field_input_DBL(char *value, void *start_addr) {
/*
	double dbl_value = strtod (value, NULL);
	double *double_cast = (double *) start_addr;
	*double_cast = dbl_value;
*/
	mara_fltR parsed_flt = __mara_new_parsed_float (value);

	double *dbl_cast = (double *) start_addr;
	*dbl_cast = parsed_flt->val;

	int *int_cast = (int *)( (char *) start_addr + sizeof(double) );
	*int_cast = parsed_flt->frac_digs;

	__teal_free(parsed_flt);
	return 0;
}

int 
teal_write_field_input_BLN(char *value, void *start_addr) {

	char *temp_buffer = teal_str_dup (value);
	bool *bool_cast = (bool *) start_addr;

	for (size_t i = 0; temp_buffer[i] != '\0'; i++) {
		temp_buffer[i] = toupper (temp_buffer[i]);
	}

	if (teal_str_same (temp_buffer, "TRUE")) {
		*bool_cast = true;
	} else {
		*bool_cast = false;
	}

	__teal_free (temp_buffer);
	return 0;
}

int 
teal_write_field_input_DATE (char *value, void *start_addr) {

	size_t count = 0;
	char **split = teal_new_arr_from_str (value, "-", &count);

	uint16_t *uint16_cast = (uint16_t *) start_addr;
	*uint16_cast = (uint16_t) strtoul (split[2], NULL, 10); // year

	uint8_t *uint8_cast = (uint8_t *) start_addr;
	*(uint8_cast + 2) = strtoul (split[0], NULL, 10); // month
	*(uint8_cast + 3) = strtoul (split[1], NULL, 10); // day

	teal_free_str_arr (split);
	return 0;
}

int 
teal_write_field_input_CURR(char *value, void *start_addr) {

	size_t count = 0;
	char **split = teal_new_arr_from_str (value, ".", &count);

	int64_t dollar_val = (int64_t) strtoll (split[0], NULL, 10);
	uint8_t cent_val = (uint8_t) strtoul (split[1], NULL, 10);

	teal_free_str_arr (split);

	int64_t *int64_cast = (int64_t *) start_addr;
	*int64_cast = dollar_val;

	uint8_t *uint8_cast = (uint8_t *) start_addr;
	*(uint8_cast + (sizeof(int64_t) / sizeof(uint8_t))) = cent_val;

	return 0;
}

int 
teal_write_field_input_CH (char *value, void *start_addr) {

	char *char_cast = (char *) start_addr;
	*char_cast = *value;
	return 0;
}
/*
int teal_write_field_input_REF(char *value, void *start_addr);
*/
int ( *teal_write_field_fnptrs[ META_NUM_TYPES_IMPLEMENTED ]) (char *value, void *start_addr);

int
teal_set_write_field_fnptrs (void) {
	teal_write_field_fnptrs[STR] = teal_write_field_input_STR;
	teal_write_field_fnptrs[ITR32] = teal_write_field_input_ITR32;
	teal_write_field_fnptrs[ITR64] = teal_write_field_input_ITR64;
	teal_write_field_fnptrs[FLT] = teal_write_field_input_DBL;
	teal_write_field_fnptrs[BLN] = teal_write_field_input_BLN;
	teal_write_field_fnptrs[DATE] = teal_write_field_input_DATE;
	teal_write_field_fnptrs[CURR] = teal_write_field_input_CURR;
	teal_write_field_fnptrs[CH] = teal_write_field_input_CH;

	teal_write_field_fnptrs_set = true;
	return 0;
}

teal_tabR
teal_new_table (char* label, size_t primary_index, char* schema) {

	if (false == teal_validate_input_fnptrs_set) {
		teal_set_validate_input_fnptrs ();
	}
	if (false == teal_write_field_fnptrs_set) {
		teal_set_write_field_fnptrs ();
	}
	if (false == teal_print_field_value_fnptrs_set) {
		teal_set_print_field_value_fnptrs ();
	}

	teal_tabR table = __teal_calloc (1, sizeof(struct Teal_Table));
	table->label = teal_str_dup (label);

	uuid_t bin_uuid;
	uuid_generate_random (bin_uuid);
	table->uuid = __teal_calloc (UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper (bin_uuid, table->uuid);

	char **schema_inputs = teal_new_arr_from_str (schema, " ", &(table->n_cols));
	
	table->col_labels = __teal_calloc (table->n_cols, sizeof(char *));
	table->schema = __teal_calloc (table->n_cols, sizeof(TABLE_FIELD_TYPE));
	table->field_offsets = __teal_calloc (table->n_cols, sizeof(size_t));

	for (size_t i = 0; i < table->n_cols; i++) {

		size_t type_index = teal_scan_arr_for_str ( TEAL_FIELD_TYPE_NAMES,
												schema_inputs[i]);

		if ( type_index == ARR_INDEX_OOB ) {
			goto cancel;
		}
		table->field_offsets[i] = table->n_bytes_row;
		table->schema[i] = type_index;
		table->n_bytes_row += TEAL_FIELD_TYPE_SIZES[ type_index ];
	}
	teal_free_str_arr (schema_inputs);

	table->n_bytes_schema = table->n_bytes_row;
	table->n_bytes_row += TEAL_FIELD_TYPE_SIZES[ ROW_ID ]; // add space for ROW_ID

	if (primary_index == 0) {
		table->has_primary = false;
	} else {
		table->has_primary = true;
		table->primary = primary_index;
	}
	return table;

cancel:
	teal_free_str_arr (schema_inputs);
	__teal_free (table->label);
	__teal_free (table->uuid);
	__teal_free (table->schema);
	__teal_free (table->col_labels);
	__teal_free (table);
	return NULL;
}

int
teal_table_insert_row (teal_tabR table, char *row) {

	int ret = 0;
	size_t value_count = 0;
	char **split_row = teal_new_arr_from_str (row, ",", &value_count);

	if (value_count != table->n_cols) {
		ret = 1;
		goto cleanup;
	}

	for (size_t i = 0; i < value_count; i++) {
		bool result = false;
		if (!(result = 
				(*teal_validate_input_fnptrs[ table->schema[i] ])( split_row[i] ))) {
			ret = 2;
			goto cleanup;
		}
	}
	if (!table->bytes) {
		table->bytes = __teal_calloc (DATA_BUFFER_DEFAULT_SIZE, sizeof(char));
		table->bytes_avail = DATA_BUFFER_DEFAULT_SIZE;
	}

	while (table->bytes_avail - table->bytes_used < table->n_bytes_row) {
		teal_table_grow_bytes (table);
	}

	char *writer_addr = (char *) table->bytes;
	writer_addr += table->bytes_used;

	size_t *size_t_cast = (size_t *) writer_addr;
	*size_t_cast = table->n_rows + 1; // write ROW_ID; starts at idx 1 naturally
	
	table->n_rows++;

	writer_addr += TEAL_FIELD_TYPE_SIZES[ ROW_ID ];

	for (size_t i = 0; i < value_count; i++) {
		( *teal_write_field_fnptrs[ table->schema[i] ] )( split_row[i], writer_addr );
		writer_addr += TEAL_FIELD_TYPE_SIZES[ table->schema[i] ];
	}

	table->bytes_used += table->n_bytes_row;

cleanup:
	teal_free_str_arr (split_row);
	return ret;
}

int
teal_table_grow_bytes (teal_tabR table) {

	void *new_bytes = NULL;
	size_t new_size = table->bytes_avail * DATA_BUFFER_GROWTH_FACTOR;

	if( NULL == (new_bytes = __teal_realloc (table->bytes, new_size)) ) {
		return 1;
	}

	table->bytes = new_bytes;
	table->bytes_avail = new_size;
	return 0;
}

void
teal_debug_print_table_info (teal_tabR table) {

	printf ("label: %s\n", table->label);
	printf ("uuid: %s\n", table->uuid);
	printf ("n_bytes_schema: %zu\n", table->n_bytes_schema);
	printf ("n_bytes_row: %zu\n", table->n_bytes_row);
	printf ("ncols: %zu, nrows: %zu\n", table->n_cols, table->n_rows);
	printf ("bytes_avail: %zu\n", table->bytes_avail);
	printf ("bytes_used: %zu\n", table->bytes_used);
	return;
}
