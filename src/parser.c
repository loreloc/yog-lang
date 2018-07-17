
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
	struct ast *tree = ast_make(AST_NODE_SOURCE);
	
	expect_token(ctx, TOKEN_VAR);
	
	ast_add_subtree(tree, parse_variables(ctx));

	expect_token(ctx, TOKEN_BEGIN);

	ast_add_subtree(tree, parse_statements(ctx));
	
	expect_token(ctx, TOKEN_END);

	return tree;
}

struct ast *parse_variables(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_NODE_VARIABLES);
	struct ast *subtree = tree;

	while(true)
	{
		struct symbol *sym = ctx->tok.value.sym;

		if(!accept_token(ctx, TOKEN_IDENTIFIER))
			break;

		bool valid = true;
		valid &= expect_token(ctx, TOKEN_COLON);
		valid &= expect_token(ctx, TOKEN_INT);
		valid &= expect_token(ctx, TOKEN_SEMICOLON);

		if(valid)
		{
			ast_add_subtree_symbol(subtree, sym);
			ast_add_subtree_node(subtree, AST_NODE_INT);
			subtree = ast_add_subtree_node(subtree, AST_NODE_VARIABLES);
		}
	}

	return tree;
}

struct ast *parse_statements(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_NODE_STATEMENTS);
	struct ast *subtree = tree;

	while(true)
	{
		if(check_token(ctx, TOKEN_END | TOKEN_EOF))
			break;

		struct ast *instr_tree;
		struct symbol *sym = ctx->tok.value.sym;

		if(accept_token(ctx, TOKEN_IDENTIFIER))
		{
			if(expect_token(ctx, TOKEN_EQUAL))
			{
				instr_tree = ast_make(AST_NODE_ASSIGN);
				ast_add_subtree_symbol(instr_tree, sym);
				ast_add_subtree_node(instr_tree, AST_NODE_EQUAL);
				ast_add_subtree(instr_tree, parse_expression(ctx));
			}
		}
		else if(accept_token(ctx, TOKEN_READ))
		{
			sym = ctx->tok.value.sym;
			
			if(expect_token(ctx, TOKEN_IDENTIFIER))
			{
				instr_tree = ast_make(AST_NODE_INPUT);
				ast_add_subtree_node(instr_tree, AST_NODE_READ);
				ast_add_subtree_symbol(instr_tree, sym);
			}
		}
		else if(accept_token(ctx, TOKEN_WRITE))
		{
			instr_tree = ast_make(AST_NODE_OUTPUT);
			ast_add_subtree_node(instr_tree, AST_NODE_WRITE);
			ast_add_subtree(instr_tree, parse_expression(ctx));
		}
		else
		{
			error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, TOKEN_IDENTIFIER | TOKEN_READ | TOKEN_WRITE);
			next_token(ctx);

			continue;
		}

		if(expect_token(ctx, TOKEN_SEMICOLON))
		{
			ast_add_subtree(subtree, instr_tree);
			subtree = ast_add_subtree_node(subtree, AST_NODE_STATEMENTS);
		}
		else
		{
			ast_clear(instr_tree);
		}
	}

	return tree;
}

struct ast *parse_expression(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_NODE_EXPRESSION);
	struct ast *subtree = tree;

	ast_add_subtree(tree, parse_term(ctx));

	struct token tok = ctx->tok;

	while(accept_token(ctx, TOKEN_PLUS | TOKEN_MINUS))
	{
		struct ast *binary_op = ast_make((tok.type == TOKEN_PLUS) ? AST_NODE_PLUS : AST_NODE_MINUS);
		ast_add_subtree(subtree, binary_op);

		struct ast *expression_tree = ast_make(AST_NODE_EXPRESSION);
		ast_add_subtree(expression_tree, parse_term(ctx));

		ast_add_subtree(subtree, expression_tree);
		subtree = expression_tree;
	}

	return tree;
}

struct ast *parse_term(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_NODE_TERM);
	struct ast *subtree = tree;

	ast_add_subtree(tree, parse_factor(ctx));

	struct token tok = ctx->tok;

	while(accept_token(ctx, TOKEN_MUL | TOKEN_DIV))
	{
		struct ast *binary_op = ast_make((tok.type == TOKEN_MUL) ? AST_NODE_MUL : AST_NODE_DIV);
		ast_add_subtree(tree, binary_op);

		struct ast *expression_tree = ast_make(AST_NODE_EXPRESSION);
		ast_add_subtree(expression_tree, parse_term(ctx));

		ast_add_subtree(subtree, expression_tree);
		subtree = expression_tree;
	}

	return tree;
}

struct ast *parse_factor(struct parse_context *ctx)
{
	struct ast *tree = ast_make(AST_NODE_FACTOR);
	struct token tok = ctx->tok;

	if(accept_token(ctx, TOKEN_LITERAL))
	{
		ast_add_subtree_literal(tree, tok.value.lit);
	}
	else if(accept_token(ctx, TOKEN_IDENTIFIER))
	{
		ast_add_subtree_symbol(tree, tok.value.sym);
	}
	else if(accept_token(ctx, TOKEN_PLUS))
	{
		struct ast *unary_tree = ast_make(AST_NODE_PLUS);
		ast_add_subtree(unary_tree, parse_expression(ctx));

		ast_add_subtree(tree, unary_tree);
	}
	else if(accept_token(ctx, TOKEN_MINUS))
	{
		struct ast *unary_tree = ast_make(AST_NODE_MINUS);
		ast_add_subtree(unary_tree, parse_expression(ctx));

		ast_add_subtree(tree, unary_tree);
	}
	else if(accept_token(ctx, TOKEN_LPAREN))
	{
		ast_add_subtree(tree, parse_expression(ctx));

		expect_token(ctx, TOKEN_RPAREN);
	}
	else
	{
		error_list_add_syntactic(ctx->errs, ctx->tok.loc, ctx->tok.type, TOKEN_LITERAL | TOKEN_IDENTIFIER);
		next_token(ctx);

		ast_add_subtree_literal(tree, 0);
	}

	return tree;
}

