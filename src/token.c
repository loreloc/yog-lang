
#include "token.h"

const char *token_type_str(token_type_t type)
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
		case TOKEN_ASSIGN:
			return ":=";
		case TOKEN_LPAREN:
			return "(";
		case TOKEN_RPAREN:
			return ")";
		case TOKEN_IF:
			return "if";
		case TOKEN_ELSE:
			return "else";
		case TOKEN_WHILE:
			return "while";
		case TOKEN_REPEAT:
			return "repeat";
		case TOKEN_UNTIL:
			return "until";
		case TOKEN_EQ:
			return "=";
		case TOKEN_NEQ:
			return "<>";
		case TOKEN_LT:
			return "<";
		case TOKEN_LTE:
			return "<=";
		case TOKEN_GT:
			return ">";
		case TOKEN_GTE:
			return ">=";
		default:
			return NULL;
	}
}

