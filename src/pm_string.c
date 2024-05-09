#include "pm_string.h"

size_t
pm_strlen(char* str) {
	size_t len = 0;
	for (size_t i = 0; str[i] != '\0'; i++) {
		len++;
	}
	return len;
}
