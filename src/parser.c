
#include "parser.h"

void next_token(struct parse_context *ctx);
bool check_token(struct parse_context *ctx, enum token_type types);
bool accept_token(struct parse_context *ctx, enum token_type types);
bool expect_token(struct parse_context *ctx, enum token_type types);
struct ast *parse_source(struct parse_context *ctx);
struct ast *parse_variables(struct parse_context *ctx);
struct ast *parse_statements(struct parse_context *ctx);
struct ast *parse_expression(struct parse_context *ctx);
struct ast *parse_term(struct parse_context *ctx);
struct ast *parse_factor(struct parse_context *ctx);

void parse_context_init(struct parse_context *ctx, FILE *source, struct symbol_table *st, struct error_list *errs)
{
	// initialize the lexical context
	lex_context_init(&ctx->lex_ctx, source, st, errs);

	ctx->st = st;
	ctx->errs = errs;
}

struct ast *parse(struct parse_context *ctx)
{
	// get the first token
	next_token(ctx);

	return parse_source(ctx);
}

void next_token(struct parse_context *ctx)
{
	ctx->tok = lex(&ctx->lex_ctx);
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
	
	error_list_add(ctx->errs, error_make_syntactic(ctx->tok.loc, ctx->tok.type, types));

	return false;
}

struct ast *parse_source(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_SOURCE);

	if(expect_token(ctx, TOKEN_VAR))
		ast_add_child(tree, ast_make(AST_VAR));
	
	ast_add_child(tree, parse_variables(ctx));

	if(expect_token(ctx, TOKEN_BEGIN))
		ast_add_child(tree, ast_make(AST_BEGIN));

	ast_add_child(tree, parse_statements(ctx));
	
	if(expect_token(ctx, TOKEN_END))
		ast_add_child(tree, ast_make(AST_END));

	return tree;
}

struct ast *parse_variables(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_VARIABLES);
	struct ast *subtree = tree;

	while(true)
	{
		struct symbol *sym = ctx->tok.value.sym;

		if(!accept_token(ctx, TOKEN_IDENTIFIER))
			break;

		ast_add_child(subtree, ast_make_symbol(sym));

		if(expect_token(ctx, TOKEN_COLON))
			ast_add_child(subtree, ast_make(AST_COLON));

		if(expect_token(ctx, TOKEN_INT))
			ast_add_child(subtree, ast_make(AST_INT));

		if(expect_token(ctx, TOKEN_SEMICOLON))
			ast_add_child(subtree, ast_make(AST_SEMICOLON));

		subtree = ast_add_child(subtree, ast_make(AST_VARIABLES));
	}

	return tree;
}

struct ast *parse_statements(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_STATEMENTS);
	struct ast *subtree = tree;

	while(true)
	{
		if(check_token(ctx, TOKEN_END | TOKEN_EOF))
			break;

		struct ast *stmt_tree;
		struct token tok = ctx->tok;

		if(accept_token(ctx, TOKEN_IDENTIFIER))
		{
			if(expect_token(ctx, TOKEN_EQUAL))
			{
				stmt_tree = ast_make(AST_ASSIGN);
				ast_add_child(stmt_tree, ast_make_symbol(tok.value.sym));
				ast_add_child(stmt_tree, ast_make(AST_EQUAL));
				ast_add_child(stmt_tree, parse_expression(ctx));
			}
		}
		else if(accept_token(ctx, TOKEN_READ))
		{
			tok = ctx->tok;
			
			if(expect_token(ctx, TOKEN_IDENTIFIER))
			{
				stmt_tree = ast_make(AST_INPUT);
				ast_add_child(stmt_tree, ast_make(AST_READ));
				ast_add_child(stmt_tree, ast_make_symbol(tok.value.sym));
			}
		}
		else if(accept_token(ctx, TOKEN_WRITE))
		{
			stmt_tree = ast_make(AST_OUTPUT);
			ast_add_child(stmt_tree, ast_make(AST_WRITE));
			ast_add_child(stmt_tree, parse_expression(ctx));
		}
		else
		{
			error_list_add(ctx->errs, error_make_syntactic(tok.loc, tok.type, TOKEN_IDENTIFIER | TOKEN_READ | TOKEN_WRITE));
			next_token(ctx);
			continue;
		}

		ast_add_child(subtree, stmt_tree);

		if(expect_token(ctx, TOKEN_SEMICOLON))
			ast_add_child(subtree, ast_make(AST_SEMICOLON));

		subtree = ast_add_child(subtree, ast_make(AST_STATEMENTS));
	}

	return tree;
}

struct ast *parse_expression(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_EXPRESSION);
	ast_add_child(tree, parse_term(ctx));

	struct token tok = ctx->tok;

	while(accept_token(ctx, TOKEN_PLUS | TOKEN_MINUS))
	{
		enum ast_type op = (tok.type == TOKEN_PLUS) ? AST_PLUS : AST_MINUS;
		struct ast *tmp = ast_make(op);

		ast_add_child(tmp, tree);
		ast_add_child(tmp, parse_term(ctx));

		tree = tmp;
	}

	return tree;
}

struct ast *parse_term(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_TERM);
	ast_add_child(tree, parse_factor(ctx));

	struct token tok = ctx->tok;

	while(accept_token(ctx, TOKEN_MUL | TOKEN_DIV))
	{
		enum ast_type op = (tok.type == TOKEN_MUL) ? AST_MUL : AST_DIV;
		struct ast *tmp = ast_make(op);

		ast_add_child(tmp, tree);
		ast_add_child(tmp, parse_term(ctx));

		tree = tmp;
	}

	return tree;
}

struct ast *parse_factor(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_FACTOR);

	struct token tok = ctx->tok;

	if(accept_token(ctx, TOKEN_LITERAL))
	{
		ast_add_child(tree, ast_make_literal(tok.value.lit));
	}
	else if(accept_token(ctx, TOKEN_IDENTIFIER))
	{
		ast_add_child(tree, ast_make_symbol(tok.value.sym));
	}
	else if(accept_token(ctx, TOKEN_PLUS))
	{
		struct ast *plus_tree = ast_add_child(tree, ast_make(AST_PLUS));
		ast_add_child(plus_tree, parse_expression(ctx));
	}
	else if(accept_token(ctx, TOKEN_MINUS))
	{
		struct ast *plus_tree = ast_add_child(tree, ast_make(AST_MINUS));
		ast_add_child(plus_tree, parse_expression(ctx));
	}
	else if(accept_token(ctx, TOKEN_LPAREN))
	{
		ast_add_child(tree, parse_expression(ctx));
		expect_token(ctx, TOKEN_RPAREN);
	}
	else
	{
		error_list_add(ctx->errs, error_make_syntactic(tok.loc, tok.type, TOKEN_LITERAL | TOKEN_IDENTIFIER));
		next_token(ctx);

		ast_add_child(tree, ast_make_literal(0));
	}

	return tree;
}

