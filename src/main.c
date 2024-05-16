#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

#include "pm_common.h"
#include "pm_memory.h"
#include "pm_error.h"
#include "pm_string.h"
#include "pm_interpreter.h"
#include "pm_file.h"

int main(void) {

	char *input = NULL;
	while (!pm_str_same(input, "quit")) {
		free(input);
		input = pm_new_str_stdin();
		char **split_input = pm_new_split_str(input, " ");
		for (size_t i = 0; split_input[i] != NULL; i++) {
			printf("|%s|\n", split_input[i]);
			free(split_input[i]);
		}
		free(split_input);
	}

	free(input);

	char *test = pm_new_str_from_file("test.txt");
	free(test);
	return 0;
}
