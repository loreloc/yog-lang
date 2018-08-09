
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

	FILE *source = fopen(filename, "r");
	if(!source)
	{
		printf("failed to open %s\n", filename);
		return 2;
	}

	struct error_list errs;
	error_list_init(&errs);

	struct symbol_table st;
	symbol_table_init(&st);

	struct parse_context ctx;
	parse_context_init(&ctx, source, &st, &errs);

	// parse the source code and obtain the abstract syntax tree
	struct ast *tree = parse(&ctx);

	struct semantic_context sem_ctx;
	semantic_context_init(&sem_ctx, &st, &errs, tree);

	// analyse the abstract syntax tree and obtain the instruction list
	struct instruction_list instrs = semantic_context_analyse(&sem_ctx);

	// check if a compile-time error has been occured
	if(error_list_empty(errs))
	{
		struct interpreter vm;
		interpreter_init(&vm, instrs, sem_ctx.tmp_cnt);

		// execute the instructions
		interpreter_execute(&vm);

		interpreter_clear(&vm);
	}
	else
	{
		error_list_show(errs);
	}

	// cleanup
	instruction_list_clear(&instrs);
	ast_clear(tree);
	symbol_table_clear(&st);
	error_list_clear(&errs);
	fclose(source);

	return 0;
}

