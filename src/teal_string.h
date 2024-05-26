#ifndef TEAL_STRING_H
#define TEAL_STRING_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "teal_memory.h"

#define ARR_INDEX_OOB		SIZE_MAX

size_t
teal_str_len (char *str);

char *
teal_str_dup (char *str);

char *
teal_str_chr (char *str, char c, size_t len);

size_t
teal_scan_arr_for_str (char **arr, char *str);

bool
teal_is_substr_at_addr (char* addr, char* substr);

bool
teal_str_same (char *str, char *cmp);

char *
teal_new_str_from_stdin (void);

char **
teal_new_arr_from_str (char *str, char *delim, size_t *count);

void
teal_free_str_arr (char **split_str);

#endif // TEAL_STRING_H
