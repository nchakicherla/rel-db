#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>

//#include "teal_common.h"
#include "nc_memory.h"
#include "nc_error.h"
#include "nc_string.h"
#include "nc_repl.h"
#include "nc_file.h"
#include "nc_hashmap.h"

#include "nc_scanner.h"
#include "nc_chunk.h"
#include "nc_vm.h"
#include "nc_debug.h"

int main(int argc, char **argv) {

	char *source = tl_new_str_from_file("./resources/test.lox");

	init_scanner(source);

	size_t line = -1;
	for (;;) {
		Token token = scan_token();
		if (token.line != line) {
			printf("%4zu ", token.line);
			line = token.line;
		} else {
			printf("   | ");
		}
		printf("%2d '%.*s'\n", token.type, (int)token.len, token.start); 

		if (token.type == TK_EOF) break;
	}

	tl_free(source);

	init_vm();

	Chunk chunk;
	init_chunk(&chunk);
	write_chunk(&chunk, OP_RETURN);

	disassemble_chunk(&chunk, "test chunk");
	//interpret(&chunk);
	free_vm();
	free_chunk(&chunk);

	return 0;
}



/*

#include "nc_db.h"
#include "nc_csv.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#define clrscr() printf("\033[1;1H\033[2J") // use \033 in place of \e due to error
#endif

char *TEST_LABELS[] = 
{
	"Name",
	"Age",
	"GPA",
	NULL
};


// 8/4/2024
	struct TL_Table *tab = tl_tab_new("Table1", "STR ITR32 FLT", 3, TL_NO_PRIMARY);
	tl_tab_set_labels(tab, TEST_LABELS);

	size_t a = 0;
	int ret = 0;
	char *row_to_insert = "Naveen,28,3.14159265758979e14";
	while (a < UINT16_MAX) {
		ret = tl_tab_insert_row(tab, row_to_insert, false);
		printf("%d: INSERT ROW %s\n", ret, row_to_insert); // print ret in case of failure
		a++;
	}

	tl_print_table(tab);
	tl_tab_print_info(tab);

	tl_tab_free(&tab);

	printf("str: %s, hash: %u\n", "test", FNV_1a_hash("test"));
	printf("str: %s, hash: %u\n", "test2", FNV_1a_hash("test2"));
	printf("str: %s, hash: %u\n", "test3", FNV_1a_hash("test3"));
	printf("str: %s, hash: %u\n", "test4", FNV_1a_hash("test4"));

// 5/30/2024
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
*/
