#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

//#include "teal_common.h"
#include "teal_memory.h"
#include "teal_error.h"
#include "teal_string.h"
#include "teal_interpreter.h"
#include "teal_file.h"
#include "teal_db.h"

// to do 
// 1. implement arena in memory.c
// 2. make teal_db use arena provided from main scope


char *TABLE_TEST_COLUMN_LABELS[] = 
{
	"test1",
	"test2",
	"test3",
	"test4",
	"test5",
	"test6",
	"test7",
	"test8",
	NULL};

int main (void) {

	teal_set_calloc_impl (calloc);
	teal_set_free_impl (free);

	teal_tabR table = teal_new_table ("My Table", NO_PRIMARY_KEY, "STR ITR32 ITR64 FLT BLN DATE CURR CH");
	
	if (table) {
		teal_table_update_labels (table, TABLE_TEST_COLUMN_LABELS);

		int ret1 = teal_table_insert_row (table, "test,32,64,7.2,true,3-3-2023,52.43,k");
		//teal_debug_print_table_info(table);
		int ret2 = teal_table_insert_row (table, "test,32,64,7.2,true,3-3-2023,52.43,k");
		//teal_debug_print_table_info(table);
		int ret3 = teal_table_insert_row (table, "test,32,64,7.2,true,3-3-2023,52.43,k");
		//teal_debug_print_table_info(table);
		if (ret1 || ret2 || ret3) {
			printf ("failed!\n");
		} else {
			printf ("passed!\n");
		}
		//printf("adding many rows...\n");
		for (size_t i = 4; i <= 10; i++) {
			//teal_debug_print_table_info(table);
			int ret = teal_table_insert_row (table, "test,32,64,7.2,true,3-3-2023,52.43,k");
			printf ("i: %zu, ret: %d\n", i, ret);
			printf ("addr: %p\n", teal_get_row_addr (table, i) );

			teal_print_row (table, teal_get_row_addr (table, i) );
		}
		teal_table_free (&table);
	}

	return 0;
}
