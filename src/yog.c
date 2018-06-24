
#include "scanner.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("usage:\tyog <source-filename>\n"); // TODO: check for .yog file type
		return -1;
	}

	const char *source_filename = argv[1];

	// open the source file
	FILE *source_file = fopen(source_filename, "r");
	if(!source_file)
	{
		printf("failed to open %s\n", source_filename);
		return 1;
	}

	// initialize the scanner
	scanner_t scanner;
	scanner_init(&scanner, source_file);

	// execute the lexical analysis
	while(!feof(source_file))
	{
		scan_result_t result = scan(&scanner);

		if(result.token.type == TOKEN_INVALID)
			continue;

		switch(result.token.type)
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
				printf("LITERAL:\t%ld\n", result.token.literal);
				break;

			case TOKEN_IDENTIFIER:
				printf("IDENTIFIER:\t%s\n", result.token.string);
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

	// close the source file
	fclose(source_file);

	return 0;
}

