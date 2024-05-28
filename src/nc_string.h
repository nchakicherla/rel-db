#ifndef NVC_STRING_H
#define NVC_STRING_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "nc_memory.h"

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

char *
teal_new_str_repeat (char *str, size_t n, char *delim);

char **
teal_new_split_str (char *str, char *delim, size_t *count);

char **
teal_new_split_str_safety (char *str, char *delim, char *safety, size_t *count);

void
teal_free_str_arr (char **split_str);

#endif // NVC_STRING_H
