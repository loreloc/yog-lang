
#include "parser.h"

void next_token(struct parse_context *ctx);
bool check_token(struct parse_context *ctx, enum token_type types);
bool accept_token(struct parse_context *ctx, enum token_type types);
bool expect_token(struct parse_context *ctx, enum token_type types);
void parse_source(struct parse_context *ctx);
void parse_variables(struct parse_context *ctx);
void parse_statements(struct parse_context *ctx);
struct expr_tree *parse_expression(struct parse_context *ctx);
struct expr_tree *parse_term(struct parse_context *ctx);
struct expr_tree *parse_factor(struct parse_context *ctx);

void parse_context_init(struct parse_context *ctx, FILE *source, struct symbol_table *st, struct error_list *errs)
{
	// initialize the lexical context
	lex_context_init(&ctx->lex_ctx, source);

	ctx->st = st;
	ctx->errs = errs;

	// initialize the list of instructions
	instr_list_init(&ctx->instrs);
}

void parse(struct parse_context *ctx)
{
	next_token(ctx);
	parse_source(ctx);
}

void next_token(struct parse_context *ctx)
{
	ctx->tok = lex(&ctx->lex_ctx, ctx->st, ctx->errs);
}

bool check_token(struct parse_context *ctx, enum token_type types)
{
	return ctx->tok.type & types;
}

bool accept_token(struct parse_context *ctx, enum token_type types)
{
	if(check_token(ctx, types))
	{
		next_token(ctx);
		return true;
	}

	return false;
}

bool expect_token(struct parse_context *ctx, enum token_type types)
{
	if(accept_token(ctx, types))
		return true;
	
	error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, types);

	return false;
}

void parse_source(struct parse_context *ctx)
{
	expect_token(ctx, TOKEN_VAR);

	parse_variables(ctx);

	expect_token(ctx, TOKEN_BEGIN);

	parse_statements(ctx);
	
	expect_token(ctx, TOKEN_END);
}

void parse_variables(struct parse_context *ctx)
{
	struct symbol *sym = ctx->tok.value.sym;

	while(accept_token(ctx, TOKEN_IDENTIFIER))
	{
		bool valid = true;

		valid &= expect_token(ctx, TOKEN_COLON);
		valid &= expect_token(ctx, TOKEN_INT);
		valid &= expect_token(ctx, TOKEN_SEMICOLON);

		if(valid)
		{
			sym->type = SYMBOL_INTEGER;
			sym = ctx->tok.value.sym;
		}

		sym = ctx->tok.value.sym;
	}
}

void parse_statements(struct parse_context *ctx)
{
	while(true)
	{
		struct symbol *sym = ctx->tok.value.sym;

		if(accept_token(ctx, TOKEN_IDENTIFIER))
		{
			if(expect_token(ctx, TOKEN_EQUAL))
				instr_list_add_assignment(&ctx->instrs, sym, parse_expression(ctx));
		}
		else if(accept_token(ctx, TOKEN_READ))
		{
			sym = ctx->tok.value.sym;
			
			if(expect_token(ctx, TOKEN_IDENTIFIER))
				instr_list_add_input(&ctx->instrs, sym);
		}
		else if(accept_token(ctx, TOKEN_WRITE))
		{
			instr_list_add_output(&ctx->instrs, parse_expression(ctx));
		}
		else if(check_token(ctx, TOKEN_END | TOKEN_EOF))
		{
			break;
		}
		else
		{
			error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, TOKEN_IDENTIFIER | TOKEN_READ | TOKEN_WRITE);
			next_token(ctx);
		}

		expect_token(ctx, TOKEN_SEMICOLON);
	}
}

struct expr_tree *parse_expression(struct parse_context *ctx)
{
	struct token tok;
	struct expr_tree *tree;
	
	tree = parse_term(ctx);

	tok = ctx->tok;

	while(accept_token(ctx, TOKEN_PLUS | TOKEN_MINUS))
	{
		enum operator op = (tok.type == TOKEN_PLUS) ? OP_PLUS : OP_MINUS;
		struct expr_tree *tmp = expr_tree_make_op(op);

		tmp->left = tree;
		tmp->right = parse_term(ctx);

		tree = tmp;
	}

	return tree;
}

struct expr_tree *parse_term(struct parse_context *ctx)
{
	struct token tok;
	struct expr_tree *tree;

	tree = parse_factor(ctx);

	tok = ctx->tok;

	while(accept_token(ctx, TOKEN_MUL | TOKEN_DIV))
	{
		enum operator op = (tok.type == TOKEN_MUL) ? OP_MUL : OP_DIV;
		struct expr_tree *tmp = expr_tree_make_op(op);

		tmp->left = tree;
		tmp->right = parse_factor(ctx);

		tree = tmp;
	}

	return tree;
}

struct expr_tree *parse_factor(struct parse_context *ctx)
{
	struct expr_tree *tree;
	struct token tok = ctx->tok;

	if(accept_token(ctx, TOKEN_LITERAL))
	{
		tree = expr_tree_make_lit(tok.value.lit);
	}
	else if(accept_token(ctx, TOKEN_IDENTIFIER))
	{
		tree = expr_tree_make_sym(tok.value.sym);
	}
	else
	{
		error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, TOKEN_LITERAL | TOKEN_IDENTIFIER);
		next_token(ctx);

		tree = expr_tree_make_lit(0);
	}

	return tree;
}

