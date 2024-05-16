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

int main(void) {
	/*
	char *input = NULL;
	while (!felt_str_same(input, "quit")) {
		free(input);
		input = felt_new_str_stdin();
		char **split_input = felt_new_split_str(input, " ");
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

	felt_tabR table = felt_new_table("My Table", "STR ITR32 ITR64 FLT DATE CURR TEXT BLN CH");
	if (table)
		felt_free_table(&table);

	return 0;
}
