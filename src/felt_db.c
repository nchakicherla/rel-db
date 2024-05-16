#include "felt_db.h"

const size_t FIELD_SIZES[] = {	sizeof(uint32_t), //STR
								sizeof(char),
								sizeof(char *), 
								sizeof(int32_t),
								sizeof(int64_t),
								sizeof(double) + sizeof(int16_t),

								sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t),
								sizeof(int64_t) + sizeof(uint8_t),
								sizeof(char *) };

felt_tabR
felt_new_table(char* name, char* schema) {
	felt_tabR table = felt_calloc(1, sizeof(struct Felt_Table));
	table->name = felt_str_dup(name);

	uuid_t bin_uuid;
	uuid_generate_random(bin_uuid);
	table->uuid = felt_calloc(UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper(bin_uuid, table->uuid);

	char **schema_inputs = felt_new_split_str(schema, " ");
	size_t n_fields = 0;
	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		n_fields++;
	}
	table->n_cols = n_fields;
	table->schema = felt_calloc(n_fields, sizeof(TABLE_FIELD_TYPE));
	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		if (felt_str_same(schema_inputs[i], "STR")) {
			table->schema[i] = STR;
			table->n_bytes_row += FIELD_SIZES[STR];
		} 
		else if (felt_str_same(schema_inputs[i], "ITR32")) {
			table->schema[i] = ITR32;
			table->n_bytes_row += FIELD_SIZES[ITR32];
		} 
		else if (felt_str_same(schema_inputs[i], "ITR64")) {
			table->schema[i] = ITR64;
			table->n_bytes_row += FIELD_SIZES[ITR64];
		} 
		else if (felt_str_same(schema_inputs[i], "FLT")) {
			table->schema[i] = FLT;
			table->n_bytes_row += FIELD_SIZES[FLT];
		} 
		else if (felt_str_same(schema_inputs[i], "BLN")) {
			table->schema[i] = BLN;
			table->n_bytes_row += FIELD_SIZES[BLN];
		} 
		else if (felt_str_same(schema_inputs[i], "DATE")) {
			table->schema[i] = DATE;
			table->n_bytes_row += FIELD_SIZES[DATE];
		} 
		else if (felt_str_same(schema_inputs[i], "CURR")) {
			table->schema[i] = CURR;
			table->n_bytes_row += FIELD_SIZES[CURR];
		} 
		else if (felt_str_same(schema_inputs[i], "TEXT")) {
			table->schema[i] = TEXT;
			table->n_bytes_row += FIELD_SIZES[TEXT];
		} 
		else {
			goto abort;
		}
	}
	felt_free_split_str(schema_inputs);
	return table;

abort:
	felt_free_split_str(schema_inputs);
	free(table->name);
	free(table->uuid);
	free(table->schema);
	free(table);
	return NULL;
}

void
felt_free_table(felt_tabR *felt_tabRR) {
	free((*felt_tabRR)->name);
	free((*felt_tabRR)->uuid);
	free((*felt_tabRR)->schema);
	free((*felt_tabRR)->data);
	free(*felt_tabRR);
	*felt_tabRR = NULL;
	return;
}
