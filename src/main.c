#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pm_common.h"
#include "pm_memory.h"
#include "pm_error.h"
#include "pm_string.h"
#include "pm_interpreter.h"

int main(void) {

	pm_tokR new_token = pm_calloc(1, sizeof(struct PM_Token));
	char *input = NULL;
	while (!pm_str_cmp(input, "quit")) {
		pm_free(input);
		input = pm_new_str_f_stdin(NULL);
		printf("input: %s\n", input);
	}

	pm_free(input);
	pm_free(new_token);

	char **test = pm_new_split_str(",,test, test,, test,,", ",,");
	for (size_t i = 0; test[i] != NULL; i++) {
		printf("test[%zu]: %s\n", i, test[i]);
		pm_free(test[i]);
	}
	pm_free(test);
	return 0;
}
