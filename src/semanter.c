
#include "semanter.h"

void analyse_variables(struct semantic_context *ctx);
struct instr_list analyse_statements(struct semantic_context *ctx);
struct expr_tree *convert_expression(struct ast *expression);
struct expr_tree *convert_term(struct ast *term);
struct expr_tree *convert_factor(struct ast *factor);

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
	struct ast *variables = ctx->tree->children[1];
	
	while(variables->children != NULL)
	{
		struct token id_tok   = variables->children[0]->value.tok;
		struct token type_tok = variables->children[2]->value.tok;

		if(type_tok.type == TOKEN_INT)
			id_tok.value.sym->type = SYMBOL_INTEGER;

		variables = variables->children[4];
	}
}

struct instr_list analyse_statements(struct semantic_context *ctx)
{
	struct instr_list instructions;
	instr_list_init(&instructions);

	struct ast *statements = ctx->tree->children[3];

	while(statements->children != NULL)
	{
		struct ast *stmt = statements->children[0];

		switch(stmt->value.nt)
		{
			case AST_NT_ASSIGN:
				instr_list_add(&instructions,
					instr_make_assign(stmt->children[0]->value.tok.value.sym,
						convert_expression(stmt->children[2])));
				break;

			case AST_NT_INPUT:
				instr_list_add(&instructions,
					instr_make_input(stmt->children[1]->value.tok.value.sym));
				break;

			default: // case AST_NT_OUTPUT:
				instr_list_add(&instructions,
					instr_make_output(convert_expression(stmt->children[1])));
				break;
		}

		statements = statements->children[2];
	}

	return instructions;
}

struct expr_tree *convert_expression(struct ast *expression)
{
	struct expr_tree *tree = convert_term(expression->children[0]);

	if(expression->children_cnt > 1)
	{
		enum operator op = (expression->children[1]->value.tok.type == TOKEN_PLUS) ? OP_PLUS : OP_MINUS;
		struct expr_tree *op_tree = expr_tree_make_operator(op);

		op_tree->left  = tree;
		op_tree->right = convert_expression(expression->children[2]);

		tree = op_tree;
	}

	return tree;
}

struct expr_tree *convert_term(struct ast *term)
{
	struct expr_tree *tree = convert_factor(term->children[0]);

	if(term->children_cnt > 1)
	{
		enum operator op = (term->children[1]->value.tok.type == TOKEN_MUL) ? OP_MUL : OP_DIV;
		struct expr_tree *op_tree = expr_tree_make_operator(op);

		op_tree->left  = tree;
		op_tree->right = convert_term(term->children[2]);

		tree = op_tree;
	}

	return tree;
}

struct expr_tree *convert_factor(struct ast *factor)
{
	struct expr_tree *tree;

	switch(factor->children[0]->value.tok.type)
	{
		case TOKEN_LITERAL:
			tree = expr_tree_make_literal(factor->children[0]->value.tok.value.lit);
			break;

		case TOKEN_IDENTIFIER:
			tree = expr_tree_make_symbol(factor->children[0]->value.tok.value.sym);
			break;

		case TOKEN_PLUS:
			tree = expr_tree_make_operator(OP_PLUS);
			tree->right = convert_expression(factor->children[1]);
			break;

		case TOKEN_MINUS:
			tree = expr_tree_make_operator(OP_MINUS);
			tree->right = convert_expression(factor->children[1]);
			break;

		default: // case TOKEN_LPAREN:
			tree = convert_expression(factor->children[1]);
			break;
	}

	return tree;
}

