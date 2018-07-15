
#include "semanter.h"

void analyse_variables(struct semantic_context *ctx);
struct instr_list analyse_statements(struct semantic_context *ctx);
struct expr_tree *translate_ast_expr_tree(struct ast *syn_tree);

void semantic_context_init(struct semantic_context *ctx, struct symbol_table *st, struct error_list *errs, struct ast *tree)
{
	ctx->st = st;
	ctx->errs = errs;
	ctx->tree = tree;
}

struct instr_list semantic_context_analyse(struct semantic_context *ctx)
{
	// analyse the variables and update the symbol table
	analyse_variables(ctx);

	// analyse the statements and produce the instruction list
	return analyse_statements(ctx);
}

void analyse_variables(struct semantic_context *ctx)
{
	struct ast *tmp = ctx->tree->subtrees[1];
	struct ast *id = tmp->subtrees[0];

	while(id != NULL)
	{
		id->value.sym->type = SYMBOL_INTEGER;

		tmp = tmp->subtrees[4];
		id = tmp->subtrees[0];
	}
}

struct instr_list analyse_statements(struct semantic_context *ctx)
{
	struct instr_list instrs;
	instr_list_init(&instrs);

	struct ast *tmp = ctx->tree->subtrees[3];
	struct ast *statement = tmp->subtrees[0];

	while(statement != NULL)
	{
		switch(statement->type)
		{
			case AST_NODE_ASSIGN:
				instr_list_add_assign(&instrs,
					statement->subtrees[0]->value.sym,
					translate_ast_expr_tree(statement->subtrees[2]));
				break;

			case AST_NODE_INPUT:
				instr_list_add_input(&instrs,
					statement->subtrees[1]->value.sym);
				break;

			default: // case AST_NODE_OUTPUT:
				instr_list_add_output(&instrs,
					translate_ast_expr_tree(statement->subtrees[1]));
				break;
		}

		tmp = tmp->subtrees[2];
		statement = tmp->subtrees[0];
	}

	return instrs;
}

struct expr_tree *translate_ast_expr_tree(struct ast *syn_tree)
{
	// TODO:
	return expr_tree_make_literal(0);
}

