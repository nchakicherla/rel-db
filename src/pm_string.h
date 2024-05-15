#ifndef PM_STRING_H
#define PM_STRING_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "pm_memory.h"

size_t
pm_str_len(const char *str);

char *
pm_str_dup(const char *str);

char *
pm_str_chr(char *str, char c, size_t len);

bool
pm_is_substr_at_addr(char* addr, char* substr);

bool
pm_str_cmp(char *str, char *cmp);

char *
pm_new_str_f_stdin(char* prompt);

char **
pm_new_split_str(char *str, char *delim);

#endif // PM_STRING_H
