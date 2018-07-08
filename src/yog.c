
#include "parser.h"
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
	parse(&ctx);

	// check the error list
	if(!error_list_empty(errs))
	{
		error_list_show(errs);
	}
	else
	{
		// initialize the interpreter
		struct interpreter vm;
		interpreter_init(&vm, ctx.instrs);

		// execute the interpreter
		interpreter_execute(&vm);
	}

	// cleanup
	instr_list_clear(&ctx.instrs);
	symbol_table_clear(&st);
	error_list_clear(&errs);
	fclose(source);

	return 0;
}

