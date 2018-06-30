
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

	// initialize the lexical context
	struct lex_context lex_ctx;
	lex_context_init(&lex_ctx, source);

	printf("tokens:\n");

	// execute the lexical analysis
	struct token tok;
	while(lex(&tok, &lex_ctx, &st, &err_hnd))
	{
		printf("%ld, %ld\t", tok.loc.row, tok.loc.col);

		switch(tok.type)
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
			case TOKEN_READ:
				printf("KEYWORD:\tread\n");
				break;
			case TOKEN_WRITE:
				printf("KEYWORD:\twrite\n");
				break;
			case TOKEN_COLON:
				printf("PUNCTUATION:\t:\n");
				break;
			case TOKEN_SEMICOLON:
				printf("PUNCTUATION:\t;\n");
				break;
			case TOKEN_LITERAL:
				printf("LITERAL:\t%ld\n", tok.lit);
				break;
			case TOKEN_IDENTIFIER:
				printf("IDENTIFIER:\t%s\n", tok.sym->id);
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
		printf("%lu) ", i);

		const struct symbol *tmp = st.buckets[i];
		while(tmp != NULL)
		{
			printf("%s ", tmp->id);
			tmp = tmp->next;
		}

		printf("\n");
	}

	printf("error list:\n");

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

