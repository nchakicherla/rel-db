#include "felt_db.h"

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
				sizeof(struct Felt_Reference)  };

char *TABLE_FIELD_TYPE_NAMES[] = 

			{	"STR",
				"ITR32",
				"ITR64",
				"FLT",
				"BLN",
				"DATE",
				"CURR",
				"CH",
				"ID",
				"REF",
				NULL };

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
	free((*felt_tabRR)->data);
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

int
felt_insert_row(felt_tabR table, char *row) {
	char **split_row = felt_new_str_arr_split(row, " ");
	for (size_t i = 0; split_row[i] != NULL; i++) {
		printf("%zu: %s\n", i + 1, split_row[i]);
	}
	felt_free_split_str(split_row);
	return 0;
}
