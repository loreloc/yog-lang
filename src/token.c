
#include "token.h"

const char *token_type_str(token_type_t type)
{
	int64_t type_index;

	__asm__
	(
		"bsr %1, %0\n\t"
		: "=r"(type_index)
		: "r"(type)
	);

	switch(type_index)
	{
		case 0:
			return "EOF";
		case 1:
			return "var";
		case 2:
			return "begin";
		case 3:
			return "end";
		case 4:
			return "int";
		case 5:
			return "read";
		case 6:
			return "write";
		case 7:
			return ":";
		case 8:
			return ";";
		case 9:
			return "literal";
		case 10:
			return "identifier";
		case 11:
			return "+";
		case 12:
			return "-";
		case 13:
			return "*";
		case 14:
			return "/";
		case 15:
			return ":=";
		case 16:
			return "(";
		case 17:
			return ")";
		case 18:
			return "if";
		case 19:
			return "else";
		case 20:
			return "=";
		case 21:
			return "<>";
		case 22:
			return "<";
		case 23:
			return "<=";
		case 24:
			return ">";
		case 25:
			return ">=";
		default:
			return NULL;
	}
}

