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
#include "teal_csv.h"

#define BIGNUM 50

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
/*
	teal_tabR table = teal_new_table ("My Table", NO_PRIMARY_KEY, "STR ITR32 ITR64 FLT BLN DATE CURR CH");
	
	if (table) {
		teal_table_update_labels (table, TABLE_TEST_COLUMN_LABELS);
		printf ("updated labels\n");
		int ret1 = teal_table_insert_row (table, "test,32,234567,7.2,false,4-1-1980,52.43,k");
		int ret2 = teal_table_insert_row (table, "test,48,345456,3.14,true,1-1-1700,52.43,k");
		int ret3 = teal_table_insert_row (table, "test,64,456678,6.11,false,1-3-1800,52.43,k");

		if (ret1 || ret2 || ret3) {
			printf ("failed!\n");
		} else {
			printf ("passed!\n");
		}

		printf ("adding many rows...\n");
		for (size_t i = 3; i < BIGNUM; i++) {
			teal_table_insert_row (table, "test,32,64,7.2,fAlse,3-3-2023,52.43,k");
		}
		printf ("rows added!\n");

		printf ("printing...\n");
		for (size_t i = 0; i < BIGNUM; i++) {
			teal_print_row_at_addr (table, teal_get_row_addr (table, i) );
		}

		teal_table_free (&table);
	}
*/
	printf ("reading csv...\n");
	teal_csvR csv = teal_new_csv ("./resources/kaggle/animal_crossing/rugs.csv");
	printf ("csv read!\n");

	if (csv) {

		//char *assembled_schema = teal_new_str_repeat ("STR", csv->n_cols, " ");

		teal_tabR csv_table = teal_new_table ("CSV Table", NULL, NO_PRIMARY_KEY, csv->n_cols);
		//__teal_free(assembled_schema);

		for (size_t i = 0; i < csv->n_rows; i++) {
			teal_table_insert_row (csv_table, csv->rows[i]);
			teal_print_row_at_addr (csv_table, teal_get_row_addr (csv_table, i));
		}


		teal_table_free (&csv_table);
	}

	teal_free_csv (&csv);

	return 0;
}
