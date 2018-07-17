
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
	struct ast *tmp = ast_get_subtree(ctx->tree, 1);
	struct ast *id = tmp->subtree;

	while(id != NULL)
	{
		id->value.sym->type = SYMBOL_INTEGER;

		tmp = ast_get_subtree(tmp, 4);
		id = tmp->subtree;
	}
}

struct instr_list analyse_statements(struct semantic_context *ctx)
{
	struct instr_list instrs;
	instr_list_init(&instrs);

	struct ast *tmp = ast_get_subtree(ctx->tree, 3);
	struct ast *statement = tmp->subtree;

	while(statement != NULL)
	{
		switch(statement->type)
		{
			case AST_NODE_ASSIGN:
				instr_list_add_assign(&instrs,
					statement->subtree->value.sym,
					translate_ast_expr_tree(ast_get_subtree(statement, 2)));
				break;

			case AST_NODE_INPUT:
				instr_list_add_input(&instrs,
					ast_get_subtree(statement, 1)->value.sym);
				break;

			default: // case AST_NODE_OUTPUT:
				instr_list_add_output(&instrs,
					translate_ast_expr_tree(ast_get_subtree(statement, 1)));
				break;
		}

		tmp = tmp->subtree->next->next;
		statement = tmp->subtree;
	}

	return instrs;
}

struct expr_tree *translate_ast_expr_tree(struct ast *syn_tree)
{
	struct expr_tree *tree;

	switch(syn_tree->type)
	{
		case AST_NODE_EXPRESSION:
			if(ast_get_subtree(syn_tree, 1) != NULL)
			{
				tree = expr_tree_make_operator((ast_get_subtree(syn_tree, 1)->type == AST_NODE_PLUS) ? OP_PLUS : OP_MINUS);
				tree->left = translate_ast_expr_tree(syn_tree->subtree);
				tree->right = translate_ast_expr_tree(ast_get_subtree(syn_tree, 2));
			}
			else
				tree = translate_ast_expr_tree(syn_tree->subtree);
			break;

		case AST_NODE_TERM:
			if(ast_get_subtree(syn_tree, 1) != NULL)
			{
				tree = expr_tree_make_operator((ast_get_subtree(syn_tree, 1)->type == AST_NODE_MUL) ? OP_MUL : OP_DIV);
				tree->left = translate_ast_expr_tree(syn_tree->subtree);
				tree->right = translate_ast_expr_tree(ast_get_subtree(syn_tree, 2));
			}
			else
				tree = translate_ast_expr_tree(syn_tree->subtree);
			break;

		case AST_NODE_FACTOR:
			tree = translate_ast_expr_tree(syn_tree->subtree);
			break;

		case AST_NODE_PLUS:
			tree = expr_tree_make_operator(OP_PLUS);
			tree->right = translate_ast_expr_tree(syn_tree->subtree);
			break;

		case AST_NODE_MINUS:
			tree = expr_tree_make_operator(OP_MINUS);
			tree->right = translate_ast_expr_tree(syn_tree->subtree);
			break;

		case AST_NODE_LITERAL:
			tree = expr_tree_make_literal(syn_tree->value.lit);
			break;

		default: // case AST_NODE_IDENTIFIER:
			tree = expr_tree_make_symbol(syn_tree->value.sym);
			break;
	}

	return tree;
}

