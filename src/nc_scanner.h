#ifndef NC_SCANNER_H
#define NC_SCANNER_H

#include <stdbool.h>
#include <stdlib.h>
#include "nc_string.h"

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

void init_scanner(char *source);

bool scanner_at_end();

char advance();

char peek();

char peek_next();

bool match(char expect);

Token make_token(TOKEN_TYPE type);

Token make_error_token(char *message);

void skip_whitespace();

Token string();

bool is_digit(char c);

Token number();

bool is_alpha(char c);

TOKEN_TYPE check_keyword(size_t start, size_t len, char *rest, TOKEN_TYPE type);

TOKEN_TYPE identifier_type();

Token identifier();

Token scan_token();

#endif // NC_SCANNER_H