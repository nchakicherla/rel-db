#include "pm_string.h"

size_t
pm_str_len(const char* str) {
	size_t len = 0;
	for (size_t i = 0; str[i] != '\0'; i++) {
		len++;
	}
	return len;
}

char *
pm_str_dup(char* str) {
	size_t len = pm_str_len(str);
	char *new_str = pm_calloc(len + 1, sizeof(char));
	for (size_t i = 0; i < len; i++) {
		new_str[i] = str[i];
	}
	new_str[len] = '\0';
	return new_str;
}

char *
pm_str_chr(char *str, char chr, size_t len) {
	for (size_t i = 0; i < len; i++) {
		if (str[i] == chr) {
			return &str[i];
		}
	}
	return NULL;
}

bool
pm_is_substr_at_addr(char* addr, char* substr) {
	for (size_t i = 0; i < pm_str_len(substr); i++) {
		if (substr[i] != addr[i]) {
			return false;
		}
	}
	return true;
}

bool
pm_str_same(char *str, char *cmp) {
	if (!str || !cmp) return false;
	if (str[0] != cmp[0]) return false;
	for (size_t i = 1; str[i] != '\0'; i++) {
		if(str[i] != cmp[i])
			return false;
	}
	return true;
}

char *
pm_new_str_stdin(void) {
	const size_t buff_len = 256;
	char *input = pm_calloc(buff_len, sizeof(char));
	char *end = NULL;
	while (!(end = pm_str_chr(input, '\n', buff_len))) {
		input = fgets(input, buff_len, stdin);
	}
	*end = '\0';
	return input;
}

char **
pm_new_split_str(char *str, char *delim) {
	size_t num_toks = 1;
	for (size_t i = 0; i < pm_str_len(str); i++) {
		if (pm_is_substr_at_addr(&str[i], delim)) {
			num_toks++;
			i += pm_str_len(delim) - 1;
		}
	}
	char **output = pm_calloc(num_toks + 1, sizeof(char *));
	output[num_toks] = NULL;

	char* start = str;
	char* end = str;
	for (size_t i = 0; i < num_toks; i++) {
		while (!pm_is_substr_at_addr(end, delim) && *end != '\0') {
			end++;
		}
		size_t tok_len = (size_t)(end - start);
		output[i] = pm_calloc(tok_len + 1, sizeof(char));
		for (size_t j = 0; j < tok_len; j++) {
			output[i][j] = *start;
			start++;
		}
		output[i][tok_len] = '\0';
		start = end + pm_str_len(delim);
		end = start;
	}

	return output;
}
