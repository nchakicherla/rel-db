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

	//tl_set_calloc_fn (calloc);
	//tl_set_free_fn (free);
	struct TL_Table *tableR = tl_tab_new ("My Table", "STR ITR32 ITR64 FLT BLN DATE CURR CH", 8, TL_NO_PRIMARY );
	const size_t big_num = 100;

	if (tableR) {
		tl_tab_set_labels (tableR, TABLE_TEST_COLUMN_LABELS);
		printf ("updated labels\n");
		int ret1 = tl_tab_insert_row (tableR, "test,32,234567,7.2,false,4-1-1980,52.43,k", false);
		int ret2 = tl_tab_insert_row (tableR, "test,48,345456,3.14,true,1-1-1700,52.43,k", false);
		int ret3 = tl_tab_insert_row (tableR, "test,64,456678,6.11,false,1-3-1800,52.43,k", false);

		if (ret1 || ret2 || ret3) {
			printf ("failed!\n");
		} else {
			printf ("passed!\n");
		}

		printf ("adding many rows...\n");
		for (size_t i = 3; i < big_num; i++) {
			tl_tab_insert_row (tableR, "test,32,64,7.2,fAlse,3-3-2023,52.43,k", true); // 10 000 000 rows, skipping validity takes 12s vs 17.7s
		}
		printf ("rows added!\n");


		tl_tab_free (&tableR);
	}

	printf ("reading csv...\n");
	struct TL_CSV *csvR = tl_new_csv ("./resources/kaggle/netflix_titles.csv");

	if (csvR) {
		printf ("csv read!\n");

		struct TL_Table *tableR_csv = tl_tab_new ("CSV Table", NULL, tl_csv_get_col_count (csvR), TL_NO_PRIMARY);

		for (size_t i = 0; i < 2; i++) {
			printf ("%zu\n", i);
			tl_tab_load_from_csv (tableR_csv, csvR);
		}

		tl_print_table (tableR_csv);

		tl_tab_print_info (tableR_csv);

		tl_tab_free (&tableR_csv);
		tl_free_csv (&csvR);
	} else {
		printf ("couldn't read csv!\n");
	}
	return 0;
}
