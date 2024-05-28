#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

//#include "teal_common.h"
#include "nc_memory.h"
#include "nc_error.h"
#include "nc_string.h"
#include "nc_repl.h"
#include "nc_file.h"

#include "nc_db.h"
#include "nc_csv.h"

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
	NULL
};

int main (void) {

	//tl_set_calloc_impl_fn (calloc);
	//tl_set_free_impl_fn (free);
	struct TL_Table *tableR = tl_new_table ("My Table", "STR ITR32 ITR64 FLT BLN DATE CURR CH", 8, NO_PRIMARY_KEY );
	const size_t big_num = 100;

	if (tableR) {
		tl_table_set_all_labels (tableR, TABLE_TEST_COLUMN_LABELS);
		printf ("updated labels\n");
		int ret1 = tl_table_insert_row (tableR, "test,32,234567,7.2,false,4-1-1980,52.43,k", false);
		int ret2 = tl_table_insert_row (tableR, "test,48,345456,3.14,true,1-1-1700,52.43,k", false);
		int ret3 = tl_table_insert_row (tableR, "test,64,456678,6.11,false,1-3-1800,52.43,k", false);

		if (ret1 || ret2 || ret3) {
			printf ("failed!\n");
		} else {
			printf ("passed!\n");
		}

		printf ("adding many rows...\n");
		for (size_t i = 3; i < big_num; i++) {
			tl_table_insert_row (tableR, "test,32,64,7.2,fAlse,3-3-2023,52.43,k", true); // 10 000 000 rows, skipping validity takes 12s vs 17.7s
		}
		printf ("rows added!\n");


		tl_table_free (&tableR);
	}

	printf ("reading csv...\n");
	struct TL_CSV *csvR = tl_new_csv ("./resources/kaggle/netflix_titles.csv");

	if (csvR) {
		printf ("csv read!\n");

		struct TL_Table *tableR_csv = tl_new_table ("CSV Table", NULL, tl_csv_get_col_count (csvR), NO_PRIMARY_KEY);

		for (size_t i = 0; i < 200; i++) {
			printf ("%zu\n", i);
			tl_table_load_from_csv (tableR_csv, csvR);
		}

		tl_print_table (tableR_csv);

		tl_debug_print_table_info (tableR_csv);

		tl_table_free (&tableR_csv);
		tl_free_csv (&csvR);
	} else {
		printf ("couldn't read csv!\n");
	}
	return 0;
}
