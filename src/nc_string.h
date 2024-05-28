#ifndef NVC_STRING_H
#define NVC_STRING_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "nc_memory.h"

#define ARR_INDEX_OOB		SIZE_MAX

size_t
tl_str_len (char *str);

char *
tl_str_dup (char *str);

char *
tl_str_chr (char *str, char c, size_t len);

size_t
tl_scan_str_arr_for_str (char **arr, char *str);

bool
tl_is_str_at_addr (char* addr, char* substr);

bool
tl_str_same (char *str, char *cmp);

char *
tl_new_str_f_stdin (void);

char *
tl_new_str_repeat (char *str, size_t n, char *delim);

char **
tl_new_str_arr (char *str, char *delim, size_t *count);

char **
tl_new_str_arr_safety (char *str, char *delim, char *safety, size_t *count);

void
tl_free_str_arr (char **split_str);

#endif // NVC_STRING_H
