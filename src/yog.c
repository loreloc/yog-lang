
#include "parser.h"
#include "semanter.h"
#include "interpreter.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("usage:\tyog <filename>\n");
		return 1;
	}

	const char *filename = argv[1];

	// open the source file
	FILE *source = fopen(filename, "r");
	if(!source)
	{
		printf("failed to open %s\n", filename);
		return 2;
	}

	// initialize the error handler
	struct error_list errs;
	error_list_init(&errs);

	// initialize the symbol table
	struct symbol_table st;
	symbol_table_init(&st);

	// initialize the parse context
	struct parse_context ctx;
	parse_context_init(&ctx, source, &st, &errs);

	// parse the source code
	struct ast *tree = parse(&ctx);

	// check the error list
	if(!error_list_empty(errs))
	{
		error_list_show(errs);
	}
	else
	{
		// initialize the semantic analysis context
		struct semantic_context sem_ctx;
		semantic_context_init(&sem_ctx, &st, &errs, tree);

		// analyse the abstract syntax tree
		struct instr_list instrs = semantic_context_analyse(&sem_ctx);

		// initialzie the interpreter
		struct interpreter vm;
		interpreter_init(&vm, instrs);

		// execute the instructions
		interpreter_execute(&vm);

		// clean the instruction list
		instr_list_clear(&instrs);
	}

	// cleanup
	ast_clear(tree);
	symbol_table_clear(&st);
	error_list_clear(&errs);
	fclose(source);

	return 0;
}

