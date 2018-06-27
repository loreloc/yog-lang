
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

	// initialize the symbol table
	struct symbol_table st;
	symbol_table_init(&st);

	// initialize the scanner
	struct scanner scn;
	scanner_init(&scn, source);

	// execute the lexical analysis
	while(!feof(source))
	{
		struct scan_result res;

		if(!scan(&res, &scn, &st, &err_hnd))
			continue;

		printf("%ld, %ld\t", res.loc.row, res.loc.col);

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

	printf("\nsymbol table:\n");

	// print the symbol table
	for(size_t i = 0; i < st.buckets_cnt; ++i)
	{
		const struct symbol *tmp = st.buckets[i];

		while(tmp != NULL)
		{
			printf("%s\t\t", tmp->id);

			switch(tmp->type)
			{
				case SYMBOL_UNKNOW:
					printf("unknow");
					break;
				default: // case SYMBOL_INTEGER:
					printf("integer");
					break;
			}

			printf("\n");

			tmp = tmp->next;
		}
	}

	printf("\nerror list:\n");

	// print the error list
	struct error *tmp = err_hnd.head;
	while(tmp != NULL)
	{
		printf("LEXICAL ERROR \"%s\" at %lu, %lu\n", tmp->msg, tmp->loc.row, tmp->loc.col);

		tmp = tmp->next;
	}

	symbol_table_clear(&st);
	error_handler_clear(&err_hnd);
	fclose(source);

	return 0;
}

