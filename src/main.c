#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

#include "felt_common.h"
#include "felt_memory.h"
#include "felt_error.h"
#include "felt_string.h"
#include "felt_interpreter.h"
#include "felt_file.h"
#include "felt_db.h"

char *TABLE_TEST_COLUMN_LABELS[] = 

			{	"test1",
				"test2",
				"test3",
				"test4",
				"test5",
				"test6",
				"test7",
				"test8",
				NULL};

int main(void) {
	/*
	char *input = NULL;
	while (!felt_str_same(input, "quit")) {
		free(input);
		input = felt_new_str_stdin();
		char **split_input = felt_new_str_arr_split(input, " ");
		for (size_t i = 0; split_input[i] != NULL; i++) {
			printf("|%s|\n", split_input[i]);
			free(split_input[i]);
		}
		free(split_input);
	}

	free(input);
	*/
	char *test = felt_new_str_from_file("test.txt");
	free(test);

	felt_tabR table = felt_new_table(	"My Table", 
										NO_PRIMARY, 
										"STR ITR32 ITR64 FLT DATE CURR BLN CH");
	
	if (table) {
		felt_update_table_column_labels(table, TABLE_TEST_COLUMN_LABELS);
		for (size_t i = 0; i < table->n_cols; i++) {
			if(table->primary_specified && i == table->primary) {
				printf("*");
			}
			printf("%zu: %s\n", i, table->col_labels[i]);
		}
		felt_insert_row(table, "test row");
		felt_free_table(&table);
	}

	return 0;
}
