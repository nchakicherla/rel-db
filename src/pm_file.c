#include "pm_file.h"

char *
pm_new_str_from_file(char *name) {
	FILE* file = NULL;
	char *output = NULL;
	long size;

	file = fopen(name, "r");
	if (!file)
		return NULL;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	output = pm_calloc(size + 1, sizeof(char));
	fread(output, size, 1, file);
	output[size] = '\0';
	
	fclose(file);
	return output;
}