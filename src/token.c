
#include "token.h"

const char *token_type_str(enum token_type type)
{
	switch(type)
	{
		case TOKEN_EOF:
			return "EOF";
		case TOKEN_VAR:
			return "var";
		case TOKEN_BEGIN:
			return "begin";
		case TOKEN_END:
			return "end";
		case TOKEN_INT:
			return "int";
		case TOKEN_READ:
			return "read";
		case TOKEN_WRITE:
			return "write";
		case TOKEN_COLON:
			return ":";
		case TOKEN_SEMICOLON:
			return ";";
		case TOKEN_LITERAL:
			return "literal";
		case TOKEN_IDENTIFIER:
			return "identifier";
		case TOKEN_PLUS:
			return "+";
		case TOKEN_MINUS:
			return "-";
		case TOKEN_MUL:
			return "*";
		case TOKEN_DIV:
			return "/";
		default: // case TOKEN_EQUAL:
			return "=";
	}
}

