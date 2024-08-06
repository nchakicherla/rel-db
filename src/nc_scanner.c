#include "nc_scanner.h"

Scanner scanner;

void init_scanner(char *source) {
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}

bool scanner_at_end() {
	return *scanner.current == '\0';
}

char advance() {
	scanner.current++;
	return scanner.current[-1];
}

char peek() {
	return *scanner.current;
}

char peek_next() {
	if (scanner_at_end()) {
		return '\0';
	}
	return scanner.current[1];
}

bool match(char expect) {
	if (scanner_at_end())  {
		return false;
	}
	if (*scanner.current != expect)  {
		return false;
	}
	
	scanner.current++;
	return true;
}

Token make_token(TOKEN_TYPE type) {
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.len = (size_t)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

Token make_error_token(char *message) {
	Token token;
	token.type = TK_ERROR;
	token.start = message;
	token.len = tl_str_len(message);
	token.line = scanner.line;
	return token;
}

void skip_whitespace() {
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

Token string() {
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

bool is_digit(char c) {
	return c >= '0' && c <= '9';
}

Token number() {
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

bool is_alpha(char c) {
   	return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
		c == '_';
}

TOKEN_TYPE check_keyword(size_t start, size_t len, char *rest, TOKEN_TYPE type) {
	if (scanner.current - scanner.start == start + len &&
		memcmp(scanner.start + start, rest, len) == 0) {
		return type;
	}

	return TK_IDENTIFIER;
}

TOKEN_TYPE identifier_type() {

	switch (scanner.start[0]) {
		case 'a': return check_keyword(1, 2, "nd", TK_AND);
		case 'c': return check_keyword(1, 4, "lass", TK_CLASS);
		case 'e': return check_keyword(1, 3, "lse", TK_ELSE);
		case 'f': {
			if (scanner.current - scanner.start > 1) {
				switch (scanner.start[1]) {
					case 'a': return check_keyword(2, 3, "lse", TK_FALSE);
					case 'n': return TK_FN;
					case 'o': return check_keyword(2, 1, "o", TK_FOR);
				}
			}
		}
		case 'i': return check_keyword(1, 1, "f", TK_IF);
		case 'n': return check_keyword(1, 2, "il", TK_NIL);
		case 'o': return check_keyword(1, 2, "r", TK_OR);
		case 'p': return check_keyword(1, 2, "rint", TK_PRINT);
		case 'r': return check_keyword(1, 2, "eturn", TK_RETURN);
		case 's': return check_keyword(1, 2, "uper", TK_SUPER);
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

Token identifier() {
	while (is_alpha(peek()) || is_digit(peek())) {
		advance();
	}
	return make_token(identifier_type());
}

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