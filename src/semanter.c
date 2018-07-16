
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
	struct ast *tmp = ctx->tree->subtree;
	struct ast *id = tmp->subtree;

	while(id != NULL)
	{
		id->value.sym->type = SYMBOL_INTEGER;

		tmp = tmp->subtree->next->next;
		id = tmp->subtree;
	}
}

struct instr_list analyse_statements(struct semantic_context *ctx)
{
	struct instr_list instrs;
	instr_list_init(&instrs);

	struct ast *tmp = ctx->tree->subtree->next;
	struct ast *statement = tmp->subtree;

	while(statement != NULL)
	{
		switch(statement->type)
		{
			case AST_NODE_ASSIGN:
				instr_list_add_assign(&instrs,
					statement->subtree->value.sym,
					translate_ast_expr_tree(statement->subtree->next));
				break;

			case AST_NODE_INPUT:
				instr_list_add_input(&instrs,
					statement->subtree->next->value.sym);
				break;

			default: // case AST_NODE_OUTPUT:
				instr_list_add_output(&instrs,
					translate_ast_expr_tree(statement->subtree->next));
				break;
		}

		tmp = tmp->subtree->next;
		statement = tmp->subtree;
	}

	return instrs;
}

struct expr_tree *translate_ast_expr_tree(struct ast *syn_tree)
{
	struct expr_tree *tree;

	struct ast *syn_subtree = syn_tree->subtree;

	switch(syn_tree->type)
	{
		case AST_NODE_EXPRESSION:
			if(syn_subtree->next != NULL)
			{
				tree = expr_tree_make_operator((syn_subtree->next->type == AST_NODE_PLUS) ? OP_PLUS : OP_MINUS);
				tree->left = translate_ast_expr_tree(syn_subtree);
				tree->right = translate_ast_expr_tree(syn_subtree->next->next);
			}
			else
			{
				tree = translate_ast_expr_tree(syn_subtree);
			}
			break;

		case AST_NODE_TERM:
			if(syn_subtree->next != NULL)
			{
				tree = expr_tree_make_operator((syn_subtree->next->type == AST_NODE_MUL) ? OP_MUL : OP_DIV);
				tree->left = translate_ast_expr_tree(syn_subtree);
				tree->right = translate_ast_expr_tree(syn_subtree->next->next);
			}
			else
			{
				tree = translate_ast_expr_tree(syn_subtree);
			}
			break;

		case AST_NODE_FACTOR:
			tree = translate_ast_expr_tree(syn_subtree);
			break;

		case AST_NODE_PLUS:
			tree = expr_tree_make_operator(OP_PLUS);
			tree->right = translate_ast_expr_tree(syn_subtree);
			break;

		case AST_NODE_MINUS:
			tree = expr_tree_make_operator(OP_MINUS);
			tree->right = translate_ast_expr_tree(syn_subtree);
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

