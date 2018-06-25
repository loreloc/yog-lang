
#include "scanner.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("usage:\tyog <filename>\n"); // TODO: check for .yog file type
		return -1;
	}

	const char *filename = argv[1];

	// open the source file
	FILE *source = fopen(filename, "r");
	if(!source)
	{
		printf("failed to open %s\n", filename);
		return 1;
	}

	// initialize the error handler
	struct error_handler err_hnd;
	error_handler_init(&err_hnd);

	// initialize the scanner
	struct scanner scn;
	scanner_init(&scn, source);

	// execute the lexical analysis
	while(!feof(source))
	{
		struct scan_result res;

		if(!scan(&res, &scn, &err_hnd))
			continue;

		printf("%ld %ld\t", res.loc.row, res.loc.col);

		switch(res.tok.type)
		{
			case TOKEN_VAR:
				printf("KEYWORD:\tvar\n");
				break;
			case TOKEN_BEGIN:
				printf("KEYWORD:\tbegin\n");
				break;
			case TOKEN_END:
				printf("KEYWORD:\tend\n");
				break;
			case TOKEN_INT:
				printf("KEYWORD:\tint\n");
				break;
			case TOKEN_COLON:
				printf("PUNCTUATION:\t:\n");
				break;
			case TOKEN_SEMICOLON:
				printf("PUNCTUATION:\t;\n");
				break;
			case TOKEN_LITERAL:
				printf("LITERAL:\t%ld\n", res.tok.lit);
				break;
			case TOKEN_IDENTIFIER:
				printf("IDENTIFIER:\t%s\n", res.tok.id);
				break;
			case TOKEN_PLUS:
				printf("OPERATOR:\t+\n");
				break;
			case TOKEN_MINUS:
				printf("OPERATOR:\t-\n");
				break;
			case TOKEN_MUL:
				printf("OPERATOR:\t*\n");
				break;
			case TOKEN_DIV:
				printf("OPERATOR:\t/\n");
				break;
			default: // case TOKEN_EQUAL:
				printf("OPERATOR:\t=\n");
				break;
		}
	}

	printf("\n");

	// check for errors
	for(size_t i = 0; i < err_hnd.errors_cnt; ++i)
	{
		const struct error err = err_hnd.errors[i];
		printf("LEXICAL ERROR \"%s\" at %lu, %lu\n", err.msg, err.loc.row, err.loc.col);
	}

	// clear the error handler
	error_handler_clear(&err_hnd);

	// close the source file
	fclose(source);

	return 0;
}

