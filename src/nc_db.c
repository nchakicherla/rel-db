#include "nc_db.h"

#define NUM_TYPES 8


#define DEF_BYTES_LEN  512
#define BYTES_GROW_FACTOR  2.0f

#define MAX_STR_LEN  8192

#define MAX_FLT_FRAC_DIGS  30
#define MAX_FLT_SIG_FIGS  15
#define MAX_FLT_VAL  1e20

bool input_valid_ptrs_set = false;
bool write_field_ptrs_set = false;
bool print_field_ptrs_set = false;

typedef enum 
{
	_ROW_ID = 0,
	STR,
	ITR32,
	ITR64,
	FLT,
	BLN,
	DATE,
	CURR,
	CH,
	// pending implement
	REF,
} FIELD_TYPE;

char *TYPE_NAME_LITERALS[] = 
{
	"_ROW_ID",
	"STR",
	"ITR32",
	"ITR64",
	"FLT",
	"BLN",
	"DATE",
	"CURR",
	"CH",
	// pending implement
	"REF",
	NULL
};

const size_t TYPE_SIZES_BYTES[] = 
{
	sizeof(size_t),
	sizeof(char *),
	sizeof(int32_t),
	sizeof(int64_t),
	sizeof(double) + sizeof(int),
	sizeof(bool),
	sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t),
	sizeof(int64_t) + sizeof(uint8_t),
	sizeof(char),
	// pending implement
	sizeof(teal_ref)
};

typedef struct Teal_Table 
{
	char *label;
	char *uuid;

	FIELD_TYPE *schema;
	size_t *field_offsets;
	char **col_labels;

	bool has_primary;
	size_t primary;

	size_t n_bytes_schema;
	size_t n_bytes_row;
	size_t n_cols;
	size_t n_rows;
	size_t bytes_avail;
	size_t bytes_used;

	void *bytes;

} *teal_tabR;

typedef struct Mara_Double 
{ // taken from my mara library
	double val;
	int frac_digs;
} *mara_fltR;

mara_fltR 
mara_new_parsed_float (char *str) { // taken from my mara library

	size_t len = teal_str_len (str);

	for (size_t i = 0; i < len; i++) {
		if (!isdigit (str[i]) && str[i] != '.' && str[i] != 'e' && str[i] != '-') {
			return NULL;
		}
	}

	char *end_ptr = str;
	double val = strtod (str, &end_ptr);
	int frac_digs = 0;

	if (end_ptr == str) {
		return NULL;
	}

	char *e_loc = teal_str_chr (str, 'e', len);
	char *dec_loc = teal_str_chr (str, '.', len);

    if (dec_loc) {
        if (*(dec_loc + 1) != '\0') { // check for second decimal
            if (teal_str_chr (dec_loc + 1, '.', len - (size_t)(dec_loc - str + 1))) {
                return NULL;
            }
        }
    }
    if (e_loc) {
        if (*(e_loc + 1) != '\0') {
            if (teal_str_chr (e_loc + 1, 'e', len - (size_t)(e_loc - str + 1))) {
                return NULL;
            }            
        }
    }

    if (e_loc && *(e_loc + 1) == '-') {

        char* it = e_loc;

        if (dec_loc) {
            while (*(it - 1) != '.') {
                it--;
                frac_digs++;
            }
        }
        frac_digs += (int) strtol (e_loc + 2, NULL, 10);

    } else if (!e_loc && dec_loc) {
        frac_digs += (len - (int)(dec_loc - str + 1));
    }

    if(frac_digs > MAX_FLT_FRAC_DIGS) {
        return NULL;
    }
    if(e_loc != NULL && (size_t)(e_loc - str) - 1 > MAX_FLT_SIG_FIGS) {
        return NULL;
    }
    if(val > MAX_FLT_VAL) {
        return NULL;
    }

    mara_fltR parsed_flt = impl_calloc (1, sizeof(struct Mara_Double));

    parsed_flt->val = val;
    parsed_flt->frac_digs = frac_digs;

    return parsed_flt;
}

int 
fprint_field_STR (FILE* stream, void *addr) {

	//char *str_cast = *(char **) addr;
	if (NULL == *(char **) addr) {
		return (fprintf (stream, "[nil]"));
	} else {
		return (fprintf (stream, "%s", *(char **) addr));
	}
	//return;
}

int 
fprint_field_ITR32 (FILE* stream, void *addr) {

	return (fprintf (stream, "%" PRId32 "", *(int32_t *) addr));
}

int 
fprint_field_ITR64 (FILE* stream, void *addr) {

	return (fprintf (stream, "%" PRId64 "", *(int64_t *) addr));
}

int 
fprint_field_DBL (FILE* stream, void *addr) {

	double val = *(double *) addr;
	int frac_digs = *(int *)((char *) addr + sizeof(double));

	return (fprintf (stream, "%.*f", frac_digs, val));
}

int 
fprint_field_BLN (FILE* stream, void *addr) {

	if (true == *(bool *) addr) {
		return (fprintf (stream, "TRUE"));
	} else {
		return (fprintf (stream, "FALSE"));
	}
}

int 
fprint_field_DATE (FILE* stream, void *addr) {

	char* ptr = (char *) addr;
	int ret = 0;
	ret += fprintf (stream, "%" PRIu8 "/", *(uint8_t *)(ptr + sizeof(uint16_t))); // month
	ret += fprintf (stream, "%" PRIu8 "/", *(uint8_t *)(ptr + sizeof(uint16_t) + sizeof(uint8_t))); // day
	ret += fprintf (stream, "%" PRIu16 "", *(uint16_t *) ptr); // year
	return ret;
}

int 
fprint_field_CURR (FILE* stream, void *addr) {

	char *ptr = (char *) addr;
	int ret = 0;

	ret += fprintf (stream, "$%" PRId64 ".", *(int64_t *)ptr);
	ret += fprintf (stream, "%" PRIu8 "", *(uint8_t *)(ptr + sizeof(int64_t)));

	return ret;
}

int 
fprint_field_CH (FILE* stream, void *addr) {

	//char *ptr = (char *) addr;
	return (fprintf (stream, "%c", *(char *) addr));
}

bool 
validate_input_STR (char* value) {

	if (teal_str_len (value) >= MAX_STR_LEN) {
		return false;
	}
	return true;
}

bool 
validate_input_ITR32 (char *value) {

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
validate_input_ITR64 (char *value) {

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
validate_input_DBL (char *value) {
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
	mara_fltR parsed_flt = mara_new_parsed_float (value);

	if (NULL == parsed_flt) {
		return false;
	}

	impl_free (parsed_flt);
	return true;
}

bool 
validate_input_BLN (char *value) {

	char *test = teal_str_dup (value);

	for (size_t i = 0; test[i] != '\0'; i++) {
		test[i] = toupper (test[i]);
	}

	if ( teal_str_same (test, "TRUE") || teal_str_same (test, "FALSE") ) {
		impl_free (test);
		return true;
	}

	impl_free (test);
	return false;
}

bool 
validate_input_DATE (char *value) {

	errno = 0;
	size_t count = 0;
	char **split = teal_new_split_str (value, "-", &count);

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
validate_input_CURR (char *value) {

	errno = 0;
	bool ret = false;
	size_t count = 0;
	char **split = teal_new_split_str (value, ".", &count);

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
validate_input_CH (char* value) {

	if (teal_str_len (value) != 1) {
		return false;
	}
	return true;
}

int 
write_field_STR (char *value, void *start_addr) {

	size_t len;

	if (0 == (len = teal_str_len (value))) {
		memset (start_addr, '\0', sizeof(char *));
		return 0;
	}

	char* alloc = impl_calloc (len + 1, sizeof(char));

	for (size_t i = 0; i < len; i++) {
		alloc[i] = value[i];
	}
	alloc[len] = '\0';

	memcpy (start_addr, &alloc, sizeof(char *));
	return 0;
}

int 
write_field_ITR32 (char *value, void *start_addr) {

	long int_val = strtol (value, NULL, 10);

	int32_t *int32_cast = (int32_t *) start_addr;
	*int32_cast = (int32_t) int_val;

	return 0; 
}

int 
write_field_ITR64 (char *value, void *start_addr) {

	long long int_val = strtoll (value, NULL, 10);
	
	int64_t *int64_cast = (int64_t *) start_addr;
	*int64_cast = (int64_t) int_val;

	return 0;
}

int 
write_field_DBL(char *value, void *start_addr) {

	mara_fltR parsed_flt = mara_new_parsed_float (value);

	double *dbl_cast = (double *) start_addr;
	*dbl_cast = parsed_flt->val;

	int *int_cast = (int *)( (char *) start_addr + sizeof(double) );
	*int_cast = parsed_flt->frac_digs;

	impl_free(parsed_flt);
	return 0;
}

int 
write_field_BLN(char *value, void *start_addr) {

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

	impl_free (temp_buffer);
	return 0;
}

int 
write_field_DATE (char *value, void *start_addr) {
	// fields are written "left to right" year -> month -> day 16 + 8 + 8
	size_t count = 0;
	char **split = teal_new_split_str (value, "-", &count);

	uint16_t *uint16_cast = (uint16_t *) start_addr;
	*uint16_cast = (uint16_t) strtoul (split[2], NULL, 10); // year

	uint8_t *uint8_cast = (uint8_t *) start_addr;
	*(uint8_cast + 2) = strtoul (split[0], NULL, 10); // month
	*(uint8_cast + 3) = strtoul (split[1], NULL, 10); // day

	teal_free_str_arr (split);
	return 0;
}

int 
write_field_CURR (char *value, void *start_addr) {

	size_t count = 0;
	char **split = teal_new_split_str (value, ".", &count);

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
write_field_CH (char *value, void *start_addr) {

	char *char_cast = (char *) start_addr;
	*char_cast = *value;
	return 0;
}

int 
(*fprint_fns [NUM_TYPES] ) (FILE* stream, void *addr);

bool 
(*validation_fns [NUM_TYPES] ) (char *value);

int 
(*writing_fns [NUM_TYPES] ) (char *value, void *start_addr);

void 
set_print_fns (void) {

	fprint_fns[STR] = fprint_field_STR;
	fprint_fns[ITR32] = fprint_field_ITR32;
	fprint_fns[ITR64] = fprint_field_ITR64;
	fprint_fns[FLT] = fprint_field_DBL;
	fprint_fns[BLN] = fprint_field_BLN;
	fprint_fns[DATE] = fprint_field_DATE;
	fprint_fns[CURR] = fprint_field_CURR;
	fprint_fns[CH] = fprint_field_CH;

	print_field_ptrs_set = true;
	return;
}

void 
set_validation_fns (void) {

	validation_fns[STR] = validate_input_STR;
	validation_fns[ITR32] = validate_input_ITR32;
	validation_fns[ITR64] = validate_input_ITR64;
	validation_fns[FLT] = validate_input_DBL;
	validation_fns[BLN] = validate_input_BLN;
	validation_fns[DATE] = validate_input_DATE;
	validation_fns[CURR] = validate_input_CURR;
	validation_fns[CH] = validate_input_CH;

	input_valid_ptrs_set = true;
	return;
}

void 
set_write_fns (void) {

	writing_fns[STR] = write_field_STR;
	writing_fns[ITR32] = write_field_ITR32;
	writing_fns[ITR64] = write_field_ITR64;
	writing_fns[FLT] = write_field_DBL;
	writing_fns[BLN] = write_field_BLN;
	writing_fns[DATE] = write_field_DATE;
	writing_fns[CURR] = write_field_CURR;
	writing_fns[CH] = write_field_CH;

	write_field_ptrs_set = true;
	return;
}

int
grow_table_bytes (teal_tabR table) {

	void *new_bytes = NULL;
	size_t new_size = table->bytes_avail * BYTES_GROW_FACTOR;

	if( NULL == (new_bytes = impl_grow_alloc (table->bytes, new_size, table->bytes_avail)) ) {
		return 1;
	}

	table->bytes = new_bytes;
	table->bytes_avail = new_size;
	return 0;
}

void 
teal_print_table (teal_tabR table) {

	char *row_ptr = table->bytes;
	int n_chars = 0;
	for (size_t i = 0; i < table->n_rows; i++) {
		n_chars = teal_fprint_row (table, row_ptr, stdout);
		printf("row: %zu had %d chars\n", i + 1, n_chars);
		n_chars = 0;
		row_ptr += table->n_bytes_row;
	}
	return;
}

void *
teal_get_row_addr (teal_tabR table, size_t ind) {

	return (char *) table->bytes + ((ind) * table->n_bytes_row);
}

int 
teal_fprint_row (teal_tabR table, void *addr, FILE* stream) {

	char *ptr = addr;
	int n_chars = 0;
	FILE *output = stdout;

	if (stream) {
		output = stream;
	}

	n_chars += fprintf (output, "%zu\t",  *(size_t *) ptr); // print ROW_ID
	ptr += TYPE_SIZES_BYTES[ _ROW_ID ];

	for ( size_t i = 0; i < table->n_cols; i++ ) {

		n_chars += ( *fprint_fns[ table->schema[i] ])(output, (char *) ptr + table->field_offsets[i] );

		if (i < table->n_cols - 1) {
			n_chars += fprintf (output, ", ");
		} else {
			n_chars += fprintf (output, "\n");
		}
	}
	return n_chars;
}

teal_tabR
teal_new_table (char* label, char* schema, size_t n_cols, size_t primary_index) {

	if (!input_valid_ptrs_set) {
		set_validation_fns ();
	}
	if (!write_field_ptrs_set) {
		set_write_fns ();
	}
	if (!print_field_ptrs_set) {
		set_print_fns ();
	}

	teal_tabR table = impl_calloc (1, sizeof(struct Teal_Table));
	table->label = teal_str_dup (label);

	uuid_t bin_uuid;
	uuid_generate_random (bin_uuid);
	table->uuid = impl_calloc (UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper (bin_uuid, table->uuid);

	char **schema_inputs = NULL;

	if (schema != NULL) {
		schema_inputs = teal_new_split_str (schema, " ", &(table->n_cols));
	} else {
		table->n_cols = n_cols;
	}
	
	table->col_labels = impl_calloc (table->n_cols, sizeof(char *));
	table->schema = impl_calloc (table->n_cols, sizeof(FIELD_TYPE));
	table->field_offsets = impl_calloc (table->n_cols, sizeof(size_t));

	for (size_t i = 0; i < table->n_cols; i++) {

		size_t type_index = STR;

		if (schema) {
			type_index = teal_scan_arr_for_str ( TYPE_NAME_LITERALS, schema_inputs[i]);
		}
		if ( type_index == ARR_INDEX_OOB ) {
			goto cancel;
		}

		table->field_offsets[i] = table->n_bytes_row;
		table->schema[i] = type_index;
		table->n_bytes_row += TYPE_SIZES_BYTES[ type_index ];
	}
	if (schema_inputs) {
		teal_free_str_arr (schema_inputs);
	}

	table->n_bytes_schema = table->n_bytes_row;
	table->n_bytes_row += TYPE_SIZES_BYTES[ _ROW_ID ]; // add space for _ROW_ID

	if (primary_index == 0) {
		table->has_primary = false;
	} else {
		table->has_primary = true;
		table->primary = primary_index;
	}
	return table;

cancel:
	teal_free_str_arr (schema_inputs);
	impl_free (table->label);
	impl_free (table->uuid);
	impl_free (table->schema);
	impl_free (table->col_labels);
	impl_free (table);
	return NULL;
}

void 
teal_table_free (teal_tabR *teal_tabRR) {
	
	size_t total_offset; // free any STR allocations
	for (size_t i = 0; i < (*teal_tabRR)->n_rows; i++) {
		for (size_t j = 0; j < (*teal_tabRR)->n_cols; j++) {

			if ((*teal_tabRR)->schema[j] == STR) {
				// total_offset = prior row offset + row_id size + prior fields in row offset
				total_offset = 	(i * (*teal_tabRR)->n_bytes_row) + TYPE_SIZES_BYTES[_ROW_ID] + (*teal_tabRR)->field_offsets[j];
				impl_free ( *(char**)( (char *)((*teal_tabRR)->bytes) + total_offset));
			}
		}
	}
	impl_free ((*teal_tabRR)->label);
	impl_free ((*teal_tabRR)->uuid);
	impl_free ((*teal_tabRR)->schema);
	impl_free ((*teal_tabRR)->field_offsets);
	for (size_t i = 0; i < (*teal_tabRR)->n_cols; i++) {
		impl_free ((*teal_tabRR)->col_labels[i]);
	}
	impl_free ((*teal_tabRR)->col_labels);
	impl_free ((*teal_tabRR)->bytes);
	impl_free (*teal_tabRR);
	*teal_tabRR = NULL;
	return;
}

int 
teal_table_update_labels (teal_tabR table, char **labels) {

	for (size_t i = 0; i < table->n_cols; i++) {
		if (table->col_labels[i]) {
			impl_free (table->col_labels[i]);
		}
		table->col_labels[i] = teal_str_dup (labels[i]);
	}
	return 0;
}

int
teal_table_insert_row (teal_tabR table, char *row) {

	int ret = 0;
	size_t attr_count = 0;
	char **split_row = teal_new_split_str_safety (row, ",", "\"", &attr_count);

	if (attr_count != table->n_cols) {
		ret = 1;
		goto cleanup;
	}

	for (size_t i = 0; i < attr_count; i++) {
		bool result = false;
		if (false == (result = (*validation_fns[ table->schema[i] ])( split_row[i] ))) {
			ret = 2;
			goto cleanup;
		}
	}
	if (!table->bytes) {
		table->bytes = impl_calloc (DEF_BYTES_LEN, sizeof(char));
		table->bytes_avail = DEF_BYTES_LEN;
	}

	while (table->bytes_avail - table->bytes_used < table->n_bytes_row) {
		grow_table_bytes (table);
	}

	char *writer_addr = (char *) table->bytes;
	writer_addr += table->bytes_used;

	size_t *size_t_cast = (size_t *) writer_addr;
	*size_t_cast = table->n_rows + 1; // write _ROW_ID; starts at idx 1 naturally
	
	table->n_rows++;

	writer_addr += TYPE_SIZES_BYTES[ _ROW_ID ];

	for (size_t i = 0; i < attr_count; i++) {
		( *writing_fns[ table->schema[i] ] )( split_row[i], writer_addr );
		writer_addr += TYPE_SIZES_BYTES[ table->schema[i] ];
	}

	table->bytes_used += table->n_bytes_row;

cleanup:
	teal_free_str_arr (split_row);
	return ret;
}

void
teal_debug_print_table_info (teal_tabR table) {

	printf ("table info....");
	printf ("label: %s\n", table->label);
	printf ("uuid: %s\n", table->uuid);
	printf ("n_bytes_schema: %zu\n", table->n_bytes_schema);
	printf ("n_bytes_row: %zu\n", table->n_bytes_row);
	printf ("ncols: %zu, nrows: %zu\n", table->n_cols, table->n_rows);
	printf ("bytes_avail: %zu\n", table->bytes_avail);
	printf ("bytes_used: %zu\n", table->bytes_used);
	return;
}
