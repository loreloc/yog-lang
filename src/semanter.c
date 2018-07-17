
#include "semanter.h"

void analyse_variables(struct semantic_context *ctx);
struct instr_list analyse_statements(struct semantic_context *ctx);
struct expr_tree *convert_expression(struct ast *tree);

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
	struct ast *tmp = ctx->tree->children[1];
	
	while(tmp->children != NULL)
	{
		struct ast *var_id   = tmp->children[0];
		struct ast *var_type = tmp->children[2];

		if(var_type->type == AST_INT)
			var_id->value.sym->type = SYMBOL_INTEGER;

		tmp = tmp->children[4];
	}
}

struct instr_list analyse_statements(struct semantic_context *ctx)
{
	struct instr_list instrs;
	instr_list_init(&instrs);

	struct ast *tmp = ctx->tree->children[3];

	while(tmp->children != NULL)
	{
		struct ast *stmt = tmp->children[0];

		switch(stmt->type)
		{
			case AST_ASSIGN:
				instr_list_add_assign(&instrs, stmt->children[0]->value.sym, convert_expression(stmt->children[2]));
				break;

			case AST_INPUT:
				instr_list_add_input(&instrs, stmt->children[1]->value.sym);
				break;

			default: // case AST_OUTPUT:
				instr_list_add_output(&instrs, convert_expression(stmt->children[1]));
				break;
		}

		tmp = tmp->children[2];
	}

	return instrs;
}

struct expr_tree *convert_expression(struct ast *tree)
{
	struct expr_tree *expression;

	switch(tree->type)
	{
		case AST_EXPRESSION:
		case AST_TERM:
		case AST_FACTOR:
			expression = convert_expression(tree->children[0]);
			break;

		case AST_PLUS:
			expression = expr_tree_make_operator(OP_PLUS);
			if(tree->children_cnt == 2)
			{
				expression->left  = convert_expression(tree->children[0]);
				expression->right = convert_expression(tree->children[1]);
			}
			else // unary plus
			{
				expression->right = convert_expression(tree->children[0]);
			}
			break;

		case AST_MINUS:
			expression = expr_tree_make_operator(OP_MINUS);
			if(tree->children_cnt == 2)
			{
				expression->left  = convert_expression(tree->children[0]);
				expression->right = convert_expression(tree->children[1]);
			}
			else // unary minus
			{
				expression->right = convert_expression(tree->children[0]);
			}
			break;

		case AST_MUL:
			expression = expr_tree_make_operator(OP_MUL);
			expression->left  = convert_expression(tree->children[0]);
			expression->right = convert_expression(tree->children[1]);
			break;

		case AST_DIV:
			expression = expr_tree_make_operator(OP_DIV);
			expression->left  = convert_expression(tree->children[0]);
			expression->right = convert_expression(tree->children[1]);
			break;

		case AST_LITERAL:
			expression = expr_tree_make_literal(tree->value.lit);
			break;

		default: // case AST_IDENTIFIER:
			expression = expr_tree_make_symbol(tree->value.sym);
			break;
	}

	return expression;
}

