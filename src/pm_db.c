#include "pm_db.h"

size_t FIELD_SIZES[] = {16, 
						sizeof(int32_t),
						sizeof(int64_t),
						sizeof(double),
						sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t),
						sizeof(int64_t) + sizeof(uint8_t),
						1024 };

pm_tabR
pm_new_table(char* name, char* schema) {
	pm_tabR table = pm_calloc(1, sizeof(struct PM_Table));
	table->name = pm_str_dup(name);

	uuid_t bin_uuid;
	uuid_generate_random(bin_uuid);
	table->uuid = pm_calloc(UUID_STR_LEN + 1, sizeof(char));
	uuid_unparse_upper(bin_uuid, table->uuid);

	char **schema_inputs = pm_new_split_str(schema, " ");
	size_t n_fields = 0;
	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		n_fields++;
	}
	table->schema = pm_calloc(n_fields, sizeof(TABLE_FIELD_TYPE));
	for (size_t i = 0; schema_inputs[i] != NULL; i++) {
		if (pm_str_same(schema_inputs[i], "STR")) {
			table->schema[i] = STR;
			table->n_bytes_row += FIELD_SIZES[STR];
		} else if (pm_str_same(schema_inputs[i], "ITR32")) {
			table->schema[i] = ITR32;
			table->n_bytes_row += FIELD_SIZES[ITR32];
		} else if (pm_str_same(schema_inputs[i], "ITR64")) {
			table->schema[i] = ITR64;
			table->n_bytes_row += FIELD_SIZES[ITR64];
		} else if (pm_str_same(schema_inputs[i], "FLT")) {
			table->schema[i] = FLT;
			table->n_bytes_row += FIELD_SIZES[FLT];
		}  else if (pm_str_same(schema_inputs[i], "DATE")) {
			table->schema[i] = DATE;
			table->n_bytes_row += FIELD_SIZES[DATE];
		} else if (pm_str_same(schema_inputs[i], "CURR")) {
			table->schema[i] = CURR;
			table->n_bytes_row += FIELD_SIZES[CURR];
		} else if (pm_str_same(schema_inputs[i], "TEXT")) {
			table->schema[i] = TEXT;
			table->n_bytes_row += FIELD_SIZES[TEXT];
		} else {
			pm_free_split_str(schema_inputs);
			free(table->name);
			free(table);
			printf("INVALID TYPE PROVIDED!\n");
			return NULL;
		}
		free(schema_inputs[i]);
		printf("i: %zu, n_bytes_row: %zu\n", i, table->n_bytes_row);
	}
	free(schema_inputs);

	table->n_cols = n_fields;

	return table;
}

void
pm_free_table(pm_tabR *pm_tabRR) {
	free((*pm_tabRR)->name);
	free((*pm_tabRR)->uuid);
	free((*pm_tabRR)->schema);
	free((*pm_tabRR)->data);
	free(*pm_tabRR);
	*pm_tabRR = NULL;
	return;
}
