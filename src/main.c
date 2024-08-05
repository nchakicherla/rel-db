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


typedef enum {
	// 1-character
	TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_LSQUARE, TK_RSQUARE,
	TK_COMMA, TK_DOT, TK_MINUS, TK_PLUS, TK_SEMICOLON, TK_FSLASH, TK_STAR,

	// 1- or 2-character
	TK_BANG, TK_BANG_EQUAL,
	TK_EQUAL, TK_EQUAL_EQUAL,
	TK_GREATER, TK_GREATER_EQUAL,
	TK_LESS, TK_LESS_EQUAL,

	// literals
	TK_IDENTIFIER, TK_STRING, TK_NUM,

	// keywords
	TK_AND, TK_CLASS, TK_ELSE, TK_FALSE, TK_FN, TK_FOR, TK_IF, TK_NIL, 
	TK_OR, TK_PRINT, TK_RETURN, TK_SUPER, TK_THIS, TK_TRUE, TK_VAR, TK_WHILE,

	TK_ERROR, TK_EOF
} TOKEN_TYPE;

typedef struct Scanner {
	char *start;
	char *current;
	size_t line;
} Scanner;

typedef struct Token {
	TOKEN_TYPE type;
	char *start;
	size_t len;
	size_t line;
} Token;

Scanner scanner;

void init_scanner(char *source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

static bool scanner_at_end() {
	return *scanner.current == '\0';
}

static char advance() {
	scanner.current++;
	return scanner.current[-1];
}

static char peek() {
	return *scanner.current;
}

static char peek_next() {
	if (scanner_at_end()) {
		return '\0';
	}
	return scanner.current[1];
}

static bool match(char expect) {
	if (scanner_at_end())  {
		return false;
	}
	if (*scanner.current != expect)  {
		return false;
	}
	
	scanner.current++;
	return true;
}

static Token make_token(TOKEN_TYPE type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.len = (size_t)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

static Token make_error_token(char *message) {
	Token token;
	token.type = TK_ERROR;
	token.start = message;
	token.len = tl_str_len(message);
	token.line = scanner.line;
	return token;
}

static void skip_whitespace() {
	while (true) {
		char c = peek();
		switch(c) {
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;
			case '\n':
				scanner.line++;
				advance();
				break;
			case '/':
				if (peek_next() == '/') {
					while (peek() != '\n' && !scanner_at_end()) {
						advance();
					}
				} else {
					return;
				}
				break;
			default:
				return;
		}
	}
}

static Token string() {
	while (peek() != '"' && !scanner_at_end()) {
		if (peek() == '\n') {
			scanner.line++;
		}
		advance();
	}

	if (scanner_at_end()) {
		return make_error_token("unterminated string");
	}

	advance();
	return make_token(TK_STRING);
}

static bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

static Token number() {
	while (is_digit(peek())) {
		advance();
	}

	if (peek() == '.' && is_digit(peek_next())) {
		advance();

		while (is_digit(peek())) {
			advance();
		}
	}

	return make_token(TK_NUM);
}

static bool is_alpha(char c) {
   	return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
		c == '_';
}

static TOKEN_TYPE check_keyword(size_t start, size_t len, char *rest, TOKEN_TYPE type) {
	if (scanner.current - scanner.start == start + len &&
		memcmp(scanner.start + start, rest, len) == 0) {
		return type;
	}

	return TK_IDENTIFIER;
}

static TOKEN_TYPE identifier_type() {

	switch (scanner.start[0]) {
		case 'a': return check_keyword(1, 2, "nd", TK_AND);
		case 'c': return check_keyword(1, 4, "lass", TK_AND);
		case 'e': return check_keyword(1, 3, "lse", TK_AND);
		case 'f': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return check_keyword(2, 3, "lse", TK_FALSE);
					case 'n': return TK_FN;
					case 'o': return check_keyword(2, 1, "o", TK_FOR);
				}
			}
		}
		case 'i': return check_keyword(1, 1, "f", TK_AND);
		case 'n': return check_keyword(1, 2, "il", TK_NIL);
		case 'o': return check_keyword(1, 2, "r", TK_AND);
		case 'p': return check_keyword(1, 2, "rint", TK_AND);
		case 'r': return check_keyword(1, 2, "eturn", TK_AND);
		case 's': return check_keyword(1, 2, "uper", TK_AND);
		case 't': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'h': return check_keyword(2, 2, "is", TK_THIS);
					case 'r': return check_keyword(2, 2, "ue", TK_TRUE);
				}
			}
		}
		case 'v': return check_keyword(1, 2, "ar", TK_AND);
		case 'w': return check_keyword(1, 2, "hile", TK_AND);


	}

	return TK_IDENTIFIER;
}

static Token identifier() {
	while (is_alpha(peek()) || is_digit(peek())) {
		advance();
	}
	return make_token(identifier_type());
}
/*
	TK_AND, TK_CLASS, TK_ELSE, TK_FALSE, TK_FN, TK_FOR, TK_IF, TK_NIL, 
	TK_OR, TK_PRINT, TK_RETURN, TK_SUPER, TK_THIS, TK_TRUE, TK_VAR, TK_WHILE,

*/

Token scan_token() {

	skip_whitespace();

	scanner.start = scanner.current;

	if (scanner_at_end()) return make_token(TK_EOF);

	char c = advance();

	if (is_alpha(c)) {
		return identifier();
	}
	if (is_digit(c)) {
		return number();
	}

	switch(c) {
		case '(': return make_token(TK_LPAREN);
		case ')': return make_token(TK_RPAREN);
		case '{': return make_token(TK_LBRACE);
		case '}': return make_token(TK_RBRACE);
		case '[': return make_token(TK_LSQUARE);
		case ']': return make_token(TK_RSQUARE);
		case ',': return make_token(TK_COMMA);
		case '.': return make_token(TK_DOT);
		case '-': return make_token(TK_MINUS);
		case '+': return make_token(TK_PLUS);
		case ';': return make_token(TK_SEMICOLON);
		case '/': return make_token(TK_FSLASH);
		case '*': return make_token(TK_STAR);

		case '1':
			return make_token (
				match('=') ? TK_BANG_EQUAL : TK_BANG);
		case '=':
			return make_token (
				match('=') ? TK_EQUAL_EQUAL : TK_EQUAL);
		case '<':
			return make_token (
				match('=') ? TK_LESS_EQUAL : TK_LESS);
		case '>':
			return make_token (
				match('=') ? TK_GREATER_EQUAL : TK_GREATER);
		case '"': 
			return string();

	}

	return make_error_token("unexpected character");
}

int main(int argc, char **argv) {

	char *source = tl_new_str_from_file("./resources/test.lox");

	init_scanner(source);

	size_t line = SIZE_MAX;
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
