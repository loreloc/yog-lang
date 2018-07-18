
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
	ctx->prev_tok = ctx->curr_tok;
	ctx->curr_tok = lex(&ctx->lex_ctx);
}

bool check_token(struct parse_context *ctx, enum token_type types)
{
	return ctx->curr_tok.type & types;
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
	
	error_list_add(ctx->errs, error_make_syntactic(ctx->curr_tok.loc, ctx->curr_tok.type, types));

	return false;
}

struct ast *parse_source(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_SOURCE);

	if(expect_token(ctx, TOKEN_VAR))
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
	
	ast_add_child(tree, parse_variables(ctx));

	if(expect_token(ctx, TOKEN_BEGIN))
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));

	ast_add_child(tree, parse_statements(ctx));
	
	if(expect_token(ctx, TOKEN_END))
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));

	return tree;
}

struct ast *parse_variables(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_VARIABLES);
	struct ast *subtree = tree;

	while(true)
	{
		if(!accept_token(ctx, TOKEN_IDENTIFIER))
			break;

		ast_add_child(subtree, ast_make_terminal(ctx->prev_tok));

		if(expect_token(ctx, TOKEN_COLON))
			ast_add_child(subtree, ast_make_terminal(ctx->prev_tok));

		if(expect_token(ctx, TOKEN_INT))
			ast_add_child(subtree, ast_make_terminal(ctx->prev_tok));

		if(expect_token(ctx, TOKEN_SEMICOLON))
			ast_add_child(subtree, ast_make_terminal(ctx->prev_tok));

		subtree = ast_add_child(subtree, ast_make_nonterminal(AST_NT_VARIABLES));
	}

	return tree;
}

struct ast *parse_statements(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_STATEMENTS);
	struct ast *subtree = tree;

	while(true)
	{
		if(check_token(ctx, TOKEN_END | TOKEN_EOF))
			break;

		struct ast *stmt_tree;

		if(accept_token(ctx, TOKEN_IDENTIFIER))
		{
			struct token id_tok = ctx->prev_tok;

			if(expect_token(ctx, TOKEN_EQUAL))
			{
				stmt_tree = ast_make_nonterminal(AST_NT_ASSIGN);
				ast_add_child(stmt_tree, ast_make_terminal(id_tok));
				ast_add_child(stmt_tree, ast_make_terminal(ctx->prev_tok));
				ast_add_child(stmt_tree, parse_expression(ctx));
			}
		}
		else if(accept_token(ctx, TOKEN_READ))
		{
			struct token read_tok = ctx->prev_tok;

			if(expect_token(ctx, TOKEN_IDENTIFIER))
			{
				struct token id_tok = ctx->prev_tok;

				stmt_tree = ast_make_nonterminal(AST_NT_INPUT);
				ast_add_child(stmt_tree, ast_make_terminal(read_tok));
				ast_add_child(stmt_tree, ast_make_terminal(id_tok));
			}
		}
		else if(accept_token(ctx, TOKEN_WRITE))
		{
			struct token write_tok = ctx->prev_tok;

			stmt_tree = ast_make_nonterminal(AST_NT_OUTPUT);
			ast_add_child(stmt_tree, ast_make_terminal(write_tok));
			ast_add_child(stmt_tree, parse_expression(ctx));
		}
		else
		{
			error_list_add(ctx->errs, error_make_syntactic(ctx->curr_tok.loc, ctx->curr_tok.type, TOKEN_IDENTIFIER | TOKEN_READ | TOKEN_WRITE));
			next_token(ctx);
			continue;
		}

		ast_add_child(subtree, stmt_tree);

		if(expect_token(ctx, TOKEN_SEMICOLON))
			ast_add_child(subtree, ast_make_terminal(ctx->prev_tok));

		subtree = ast_add_child(subtree, ast_make_nonterminal(AST_NT_STATEMENTS));
	}

	return tree;
}

struct ast *parse_expression(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_EXPRESSION);
	ast_add_child(tree, parse_term(ctx));

	while(accept_token(ctx, TOKEN_PLUS | TOKEN_MINUS))
	{
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
		ast_add_child(tree, parse_term(ctx));
	}

	return tree;
}

struct ast *parse_term(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_TERM);
	ast_add_child(tree, parse_factor(ctx));

	while(accept_token(ctx, TOKEN_MUL | TOKEN_DIV))
	{
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
		ast_add_child(tree, parse_factor(ctx));
	}

	return tree;
}

struct ast *parse_factor(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_FACTOR);

	if(accept_token(ctx, TOKEN_LITERAL | TOKEN_IDENTIFIER))
	{
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
	}
	else if(accept_token(ctx, TOKEN_PLUS | TOKEN_MINUS))
	{
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
		ast_add_child(tree, parse_expression(ctx));
	}
	else if(accept_token(ctx, TOKEN_LPAREN))
	{
		ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
		ast_add_child(tree, parse_expression(ctx));
		
		if(expect_token(ctx, TOKEN_RPAREN))
			ast_add_child(tree, ast_make_terminal(ctx->prev_tok));
	}
	else
	{
		error_list_add(ctx->errs, error_make_syntactic(ctx->curr_tok.loc, ctx->curr_tok.type, TOKEN_LITERAL | TOKEN_IDENTIFIER));
		next_token(ctx);

		ast_add_child(tree, NULL);
	}

	return tree;
}

