
#include "parser.h"

void report_syntactic_error(struct parse_context *ctx, enum token_type exp);
void next_token(struct parse_context *ctx);
bool check_token(struct parse_context *ctx, enum token_type types);
bool accept_token(struct parse_context *ctx, struct ast *tree, enum token_type types);
void replace_token(struct parse_context *ctx, struct ast *tree, enum token_type type, enum token_type exp);
bool expect_token(struct parse_context *ctx, struct ast *tree, enum token_type type);

struct ast *parse_source(struct parse_context *ctx);
struct ast *parse_variables(struct parse_context *ctx);
struct ast *parse_statements(struct parse_context *ctx);
struct ast *parse_assign(struct parse_context *ctx);
struct ast *parse_input(struct parse_context *ctx);
struct ast *parse_output(struct parse_context *ctx);
struct ast *parse_expression(struct parse_context *ctx);
struct ast *parse_term(struct parse_context *ctx);
struct ast *parse_factor(struct parse_context *ctx);

void parse_context_init(struct parse_context *ctx, FILE *source, struct symbol_table *st, struct error_list *errs)
{
	// initialize the lexical context
	lex_context_init(&ctx->lex_ctx, source, st, errs);

	ctx->errs = errs;
}

struct ast *parse(struct parse_context *ctx)
{
	// get the first token
	next_token(ctx);

	return parse_source(ctx);
}

void report_syntactic_error(struct parse_context *ctx, enum token_type exp)
{
	// add a new syntactic error to the error list
	error_list_add(ctx->errs, error_make_unexpected_token(ctx->curr_tok.loc, ctx->curr_tok.type, exp));
}

void next_token(struct parse_context *ctx)
{
	// save the current token and get the next token
	ctx->prev_tok = ctx->curr_tok;
	ctx->curr_tok = lex(&ctx->lex_ctx);
}

bool check_token(struct parse_context *ctx, enum token_type types)
{
	return ctx->curr_tok.type & types;
}

bool accept_token(struct parse_context *ctx, struct ast *tree, enum token_type types)
{
	// if the token type is one of types
	if(check_token(ctx, types))
	{
		// add a new token child to the abstract syntax tree node
		ast_add_child(tree, ast_make_terminal(ctx->curr_tok));
		next_token(ctx);
		return true;
	}

	return false;
}

void replace_token(struct parse_context *ctx, struct ast *tree, enum token_type type, enum token_type exp)
{
	// initialize the error recovering token
	struct token tok;
	tok.loc = ctx->curr_tok.loc;
	tok.type = type;
	tok.sym = NULL;

	// add the error recovering token as a child to the abstract syntax tree node
	ast_add_child(tree, ast_make_terminal(tok));

	// report the error
	report_syntactic_error(ctx, exp);
}

bool expect_token(struct parse_context *ctx, struct ast *tree, enum token_type type)
{
	// check if the token type is equal to type
	if(accept_token(ctx, tree, type))
		return true;
	
	// replace the token with the expected token and report the syntax error
	replace_token(ctx, tree, type, type);

	return false;
}

struct ast *parse_source(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_SOURCE);

	expect_token(ctx, tree, TOKEN_VAR);
	ast_add_child(tree, parse_variables(ctx));
	expect_token(ctx, tree, TOKEN_BEGIN);
	ast_add_child(tree, parse_statements(ctx));
	expect_token(ctx, tree, TOKEN_END);

	return tree;
}

struct ast *parse_variables(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_VARIABLES);

	while(!check_token(ctx, TOKEN_EOF | TOKEN_BEGIN))
	{
		if(accept_token(ctx, tree, TOKEN_IDENTIFIER))
		{
			expect_token(ctx, tree, TOKEN_COLON);
			expect_token(ctx, tree, TOKEN_INT);
			expect_token(ctx, tree, TOKEN_SEMICOLON);
		}
		else
		{
			report_syntactic_error(ctx, TOKEN_IDENTIFIER);
			next_token(ctx);
		}
	}

	return tree;
}

struct ast *parse_statements(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_STATEMENTS);

	while(!check_token(ctx, TOKEN_EOF | TOKEN_END))
	{
		if(check_token(ctx, TOKEN_IDENTIFIER))
		{
			ast_add_child(tree, parse_assign(ctx));
		}
		else if(check_token(ctx, TOKEN_READ))
		{
			ast_add_child(tree, parse_input(ctx));
		}
		else if(check_token(ctx, TOKEN_WRITE))
		{
			ast_add_child(tree, parse_output(ctx));
		}
		else if(check_token(ctx, TOKEN_SEMICOLON))
		{
			// nothing
		}
		else
		{
			report_syntactic_error(ctx, TOKEN_IDENTIFIER | TOKEN_READ | TOKEN_WRITE);
			next_token(ctx);
			continue;
		}

		expect_token(ctx, tree, TOKEN_SEMICOLON);
	}

	return tree;
}

struct ast *parse_assign(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_ASSIGN);

	expect_token(ctx, tree, TOKEN_IDENTIFIER);
	expect_token(ctx, tree, TOKEN_EQUAL);
	ast_add_child(tree, parse_expression(ctx));

	return tree;
}

struct ast *parse_input(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_INPUT);

	expect_token(ctx, tree, TOKEN_READ);
	expect_token(ctx, tree, TOKEN_IDENTIFIER);

	return tree;
}

struct ast *parse_output(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_OUTPUT);

	expect_token(ctx, tree, TOKEN_WRITE);
	ast_add_child(tree, parse_expression(ctx));

	return tree;
}

struct ast *parse_expression(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_EXPRESSION);
	ast_add_child(tree, parse_term(ctx));

	while(accept_token(ctx, tree, TOKEN_PLUS | TOKEN_MINUS))
		ast_add_child(tree, parse_term(ctx));

	return tree;
}

struct ast *parse_term(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_TERM);
	ast_add_child(tree, parse_factor(ctx));

	while(accept_token(ctx, tree, TOKEN_MUL | TOKEN_DIV))
		ast_add_child(tree, parse_factor(ctx));

	return tree;
}

struct ast *parse_factor(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NT_FACTOR);

	if(accept_token(ctx, tree, TOKEN_LITERAL | TOKEN_IDENTIFIER))
	{
		// nothing
	}
	else if(accept_token(ctx, tree, TOKEN_PLUS | TOKEN_MINUS))
	{
		ast_add_child(tree, parse_expression(ctx));
	}
	else if(accept_token(ctx, tree, TOKEN_LPAREN))
	{
		ast_add_child(tree, parse_expression(ctx));
		
		expect_token(ctx, tree, TOKEN_RPAREN);
	}
	else
	{
		replace_token(ctx, tree, TOKEN_LITERAL,
			TOKEN_LITERAL | TOKEN_IDENTIFIER | TOKEN_PLUS | TOKEN_MINUS | TOKEN_LPAREN);
		next_token(ctx);
	}

	return tree;
}

