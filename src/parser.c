
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
	
	error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, types);

	return false;
}

struct ast *parse_source(struct parse_context *ctx)
{
	struct ast *var_tree = NULL;
	struct ast *begin_tree = NULL;
	struct ast *end_tree = NULL;
	
	if(expect_token(ctx, TOKEN_VAR))
		var_tree = ast_make_terminal(AST_NODE_VAR);
	
	struct ast *variables_tree = parse_variables(ctx);

	if(expect_token(ctx, TOKEN_BEGIN))
		begin_tree = ast_make_terminal(AST_NODE_BEGIN);

	struct ast *statements_tree = parse_statements(ctx);
	
	if(expect_token(ctx, TOKEN_END))
		end_tree = ast_make_terminal(AST_NODE_END);

	return ast_make_nonterminal(AST_NODE_SOURCE, 5,
		var_tree, variables_tree, begin_tree, statements_tree, end_tree);
}

struct ast *parse_variables(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NODE_VARIABLES, 5,
		NULL, NULL, NULL, NULL, NULL);

	struct ast *subtree = tree;

	while(true)
	{
		struct symbol *sym = ctx->tok.value.sym;

		if(!accept_token(ctx, TOKEN_IDENTIFIER))
			break;

		struct ast *identifier_tree = ast_make_terminal(AST_NODE_IDENTIFIER);
		identifier_tree->value.sym = sym;

		struct ast *colon_tree = NULL;
		struct ast *int_tree = NULL;
		struct ast *semicolon_tree = NULL;

		if(expect_token(ctx, TOKEN_COLON))
			colon_tree = ast_make_terminal(AST_NODE_COLON);

		if(expect_token(ctx, TOKEN_INT))
			int_tree = ast_make_terminal(AST_NODE_INT);

		if(expect_token(ctx, TOKEN_SEMICOLON))
			semicolon_tree = ast_make_terminal(AST_NODE_SEMICOLON);

		subtree->subtrees[0] = identifier_tree;
		subtree->subtrees[1] = colon_tree;
		subtree->subtrees[2] = int_tree;
		subtree->subtrees[3] = semicolon_tree;

		subtree->subtrees[4] = ast_make_nonterminal(AST_NODE_VARIABLES, 5,
			NULL, NULL, NULL, NULL, NULL);

		subtree = subtree->subtrees[4];
	}

	return tree;
}

struct ast *parse_statements(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NODE_STATEMENTS, 3,
		NULL, NULL, NULL);

	struct ast *subtree = tree;

	while(true)
	{
		if(check_token(ctx, TOKEN_END | TOKEN_EOF))
			break;

		struct symbol *sym = ctx->tok.value.sym;

		if(accept_token(ctx, TOKEN_IDENTIFIER))
		{
			if(expect_token(ctx, TOKEN_EQUAL))
			{
				struct ast *identifier_tree = ast_make_terminal(AST_NODE_IDENTIFIER);
				identifier_tree->value.sym = sym;

				subtree->subtrees[0] = ast_make_nonterminal(AST_NODE_ASSIGN, 3,
					identifier_tree, ast_make_terminal(AST_NODE_EQUAL), parse_expression(ctx));
			}
		}
		else if(accept_token(ctx, TOKEN_READ))
		{
			sym = ctx->tok.value.sym;
			
			if(expect_token(ctx, TOKEN_IDENTIFIER))
			{
				struct ast *identifier_tree = ast_make_terminal(AST_NODE_IDENTIFIER);
				identifier_tree->value.sym = sym;

				subtree->subtrees[0] = ast_make_nonterminal(AST_NODE_INPUT, 2,
					ast_make_terminal(AST_NODE_READ), identifier_tree);
			}
		}
		else if(accept_token(ctx, TOKEN_WRITE))
		{
			subtree->subtrees[0] = ast_make_nonterminal(AST_NODE_OUTPUT, 2,
				ast_make_terminal(AST_NODE_WRITE), parse_expression(ctx));
		}
		else
		{
			error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, TOKEN_IDENTIFIER | TOKEN_READ | TOKEN_WRITE);
			next_token(ctx);
			continue;
		}

		if(expect_token(ctx, TOKEN_SEMICOLON))
		{
			subtree->subtrees[1] = ast_make_terminal(AST_NODE_SEMICOLON);

			subtree->subtrees[2] = ast_make_nonterminal(AST_NODE_STATEMENTS, 3,
				NULL, NULL, NULL);

			subtree = subtree->subtrees[2];
		}
	}

	return tree;
}

struct ast *parse_expression(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NODE_EXPRESSION, 3,
		parse_term(ctx), NULL, NULL);

	struct token tok = ctx->tok;

	while(accept_token(ctx, TOKEN_PLUS | TOKEN_MINUS))
	{
		tree->subtrees[1] = ast_make_terminal((tok.type == TOKEN_PLUS) ? AST_NODE_PLUS : AST_NODE_MINUS);
		tree->subtrees[2] = parse_term(ctx);
	}

	return tree;
}

struct ast *parse_term(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NODE_TERM, 3,
		parse_factor(ctx), NULL, NULL);

	struct token tok = ctx->tok;

	while(accept_token(ctx, TOKEN_MUL | TOKEN_DIV))
	{
		tree->subtrees[1] = ast_make_terminal((tok.type == TOKEN_MUL) ? AST_NODE_MUL : AST_NODE_DIV);
		tree->subtrees[2] = parse_term(ctx);
	}

	return tree;
}

struct ast *parse_factor(struct parse_context *ctx)
{
	struct ast *tree = ast_make_nonterminal(AST_NODE_FACTOR, 1, NULL);

	struct token tok = ctx->tok;

	if(accept_token(ctx, TOKEN_LITERAL))
	{
		tree->subtrees[0] = ast_make_terminal(AST_NODE_LITERAL);
		tree->subtrees[0]->value.lit = tok.value.lit;
	}
	else if(accept_token(ctx, TOKEN_IDENTIFIER))
	{
		tree->subtrees[0] = ast_make_terminal(AST_NODE_IDENTIFIER);
		tree->subtrees[0]->value.sym = tok.value.sym;
	}
	else if(accept_token(ctx, TOKEN_PLUS))
	{
		tree->subtrees[0] = ast_make_nonterminal(AST_NODE_PLUS, 1, parse_expression(ctx));
	}
	else if(accept_token(ctx, TOKEN_MINUS))
	{
		tree->subtrees[0] = ast_make_nonterminal(AST_NODE_MINUS, 1, parse_expression(ctx));
	}
	else if(accept_token(ctx, TOKEN_LPAREN))
	{
		tree->subtrees[0] = parse_expression(ctx);

		expect_token(ctx, TOKEN_RPAREN);
	}
	else
	{
		error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, TOKEN_LITERAL | TOKEN_IDENTIFIER);
		next_token(ctx);

		tree->subtrees[0] = ast_make_terminal(AST_NODE_LITERAL);
		tree->subtrees[0]->value.lit = 0;
	}

	return tree;
}

