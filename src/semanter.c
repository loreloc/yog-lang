
#include "semanter.h"

void analyse_variables(struct semantic_context *ctx);
struct instr_list analyse_statements(struct semantic_context *ctx);

bool accept_variable(struct semantic_context *ctx, struct token tok);
void instructions_add_assign(struct instr_list* instrs, struct semantic_context *ctx, struct ast* stmt);
void instructions_add_input(struct instr_list* instrs, struct semantic_context *ctx, struct ast* stmt);
void instructions_add_output(struct instr_list* instrs, struct semantic_context *ctx, struct ast* stmt);

struct expr_tree *convert_expression(struct semantic_context *ctx, struct ast *expression);
struct expr_tree *convert_term(struct semantic_context *ctx, struct ast *term);
struct expr_tree *convert_factor(struct semantic_context *ctx, struct ast *factor);

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
	struct instr_list instrs;
	instr_list_init(&instrs);

	struct ast *statements = ctx->tree->children[3];

	while(statements->children != NULL)
	{
		struct ast *stmt = statements->children[0];

		switch(stmt->value.nt)
		{
			case AST_NT_ASSIGN:
				instructions_add_assign(&instrs, ctx, stmt);
				break;

			case AST_NT_INPUT:
				instructions_add_input(&instrs, ctx, stmt);
				break;

			default: // case AST_NT_OUTPUT:
				instructions_add_output(&instrs, ctx, stmt);
				break;
		}

		statements = statements->children[2];
	}

	return instrs;
}

bool accept_variable(struct semantic_context *ctx, struct token tok)
{
	// check if the symbol has been declared
	if(tok.value.sym->type == SYMBOL_UNKNOW)
	{
		error_list_add(ctx->errs, error_make_semantic(tok.loc, tok.value.sym));
		return false;
	}

	return true;
}

void instructions_add_assign(struct instr_list* instrs, struct semantic_context *ctx, struct ast* stmt)
{
	struct token id_tok = stmt->children[0]->value.tok;

	if(accept_variable(ctx, id_tok))
	{
		struct expr_tree *expr = convert_expression(ctx, stmt->children[2]);
		instr_list_add(instrs, instr_make_assign(id_tok.value.sym, expr));
	}
}

void instructions_add_input(struct instr_list* instrs, struct semantic_context *ctx, struct ast* stmt)
{
	struct token id_tok = stmt->children[1]->value.tok;

	if(accept_variable(ctx, id_tok))
		instr_list_add(instrs, instr_make_input(id_tok.value.sym));
}

void instructions_add_output(struct instr_list* instrs, struct semantic_context *ctx, struct ast* stmt)
{
	struct expr_tree *expr = convert_expression(ctx, stmt->children[1]);

	instr_list_add(instrs, instr_make_output(expr));
}

struct expr_tree *convert_expression(struct semantic_context *ctx, struct ast *expression)
{
	struct expr_tree *tree = convert_term(ctx, expression->children[0]);

	if(expression->children_cnt > 1)
	{
		enum operator op = (expression->children[1]->value.tok.type == TOKEN_PLUS) ? OP_PLUS : OP_MINUS;
		struct expr_tree *op_tree = expr_tree_make_operator(op);

		op_tree->left  = tree;
		op_tree->right = convert_expression(ctx, expression->children[2]);

		tree = op_tree;
	}

	return tree;
}

struct expr_tree *convert_term(struct semantic_context *ctx, struct ast *term)
{
	struct expr_tree *tree = convert_factor(ctx, term->children[0]);

	if(term->children_cnt > 1)
	{
		enum operator op = (term->children[1]->value.tok.type == TOKEN_MUL) ? OP_MUL : OP_DIV;
		struct expr_tree *op_tree = expr_tree_make_operator(op);

		op_tree->left  = tree;
		op_tree->right = convert_term(ctx, term->children[2]);

		tree = op_tree;
	}

	return tree;
}

struct expr_tree *convert_factor(struct semantic_context *ctx, struct ast *factor)
{
	struct expr_tree *tree;

	switch(factor->children[0]->value.tok.type)
	{
		case TOKEN_LITERAL:
			tree = expr_tree_make_literal(factor->children[0]->value.tok.value.lit);
			break;

		case TOKEN_IDENTIFIER:
			if(accept_variable(ctx, factor->children[0]->value.tok))
				tree = expr_tree_make_symbol(factor->children[0]->value.tok.value.sym);
			else
				tree = NULL;
			break;

		case TOKEN_PLUS:
			tree = expr_tree_make_operator(OP_PLUS);
			tree->right = convert_expression(ctx, factor->children[1]);
			break;

		case TOKEN_MINUS:
			tree = expr_tree_make_operator(OP_MINUS);
			tree->right = convert_expression(ctx, factor->children[1]);
			break;

		default: // case TOKEN_LPAREN:
			tree = convert_expression(ctx, factor->children[1]);
			break;
	}

	return tree;
}

