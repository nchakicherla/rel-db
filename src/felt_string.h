#ifndef FELT_STRING_H
#define FELT_STRING_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "felt_memory.h"

size_t
felt_str_len(char *str);

char *
felt_str_dup(char *str);

char *
felt_str_chr(char *str, char c, size_t len);

bool
felt_is_substr_at_addr(char* addr, char* substr);

bool
felt_str_same(char *str, char *cmp);

char *
felt_new_str_stdin(void);

char **
felt_new_split_str(char *str, char *delim);

void
felt_free_split_str(char **split_str);

#endif // FELT_STRING_H
