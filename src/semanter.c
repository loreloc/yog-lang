
#include "semanter.h"

bool accept_variable(struct semantic_context *ctx, struct token tok);

void analyse_variables(struct semantic_context *ctx);
struct instruction_list analyse_statements(struct semantic_context *ctx, size_t *tmp_cnt);

void analyse_assign(struct semantic_context *ctx, struct ast *assign);
void analyse_input(struct semantic_context *ctx, struct ast *input);
void analyze_output(struct semantic_context *ctx, struct ast *output);

struct operand analyse_expression(struct semantic_context *ctx, struct ast *expression);
struct operand analyse_term(struct semantic_context *ctx, struct ast *term);
struct operand analyse_factor(struct semantic_context *ctx, struct ast *factor);

void semantic_context_init(struct semantic_context *ctx, struct symbol_table *st, struct error_list *errs, struct ast *tree)
{
	ctx->st = st;
	ctx->errs = errs;
	ctx->tree = tree;

	ctx->instrs.head = NULL;
	ctx->instrs.tail = NULL;

	ctx->tmp_cnt = 0;
}

struct instruction_list semantic_context_analyse(struct semantic_context *ctx, size_t *tmp_cnt)
{
	// analyse the variables and update the symbol table
	analyse_variables(ctx);

	// analyse the statements and produce the instructio nlist
	return analyse_statements(ctx, tmp_cnt);
}

bool accept_variable(struct semantic_context *ctx, struct token tok)
{
	// check if the symbol has been declared
	if(tok.sym->type == SYMBOL_UNKNOW)
	{
		error_list_add(ctx->errs, error_make_undeclared_var(tok.loc, tok.sym));
		return false;
	}

	return true;
}

void analyse_variables(struct semantic_context *ctx)
{
	struct ast *variables = ctx->tree->children[1];

	for(size_t i = 0; i < variables->children_cnt; i += 4)
	{
		struct token id_tok   = variables->children[i  ]->tok;
		struct token type_tok = variables->children[i+2]->tok;

		if(id_tok.sym != NULL)
		{
			// check for multiple definitions
			if(id_tok.sym->type != SYMBOL_UNKNOW)
			{
				error_list_add(ctx->errs, error_make_multiple_decl(id_tok.loc, id_tok.sym->loc, id_tok.sym));
			}
			else
			{
				if(type_tok.type == TOKEN_INT)
				{
					id_tok.sym->type = SYMBOL_INTEGER;
					id_tok.sym->loc = id_tok.loc;
				}
			}
		}
	}
}

struct instruction_list analyse_statements(struct semantic_context *ctx, size_t *tmp_cnt)
{
	struct instruction_list instrs;
	instruction_list_init(&instrs);

	struct ast *statements = ctx->tree->children[3];

	for(size_t i = 0; i < statements->children_cnt; )
	{
		struct ast *stmt = statements->children[i];

		if(stmt->type == AST_NONTERMINAL)
		{
			ctx->instrs.head = NULL;
			ctx->instrs.tail = NULL;
			ctx->tmp_cnt = 0;

			switch(stmt->nt)
			{
				case AST_NT_ASSIGN:
					analyse_assign(ctx, stmt);
					break;

				case AST_NT_INPUT:
					analyse_input(ctx, stmt);
					break;

				default: // case AST_NT_OUTPUT:
					analyze_output(ctx, stmt);
					break;
			}

			// update the number of temporary variables
			if(ctx->tmp_cnt > *tmp_cnt)
				*tmp_cnt = ctx->tmp_cnt;

			// merge the instruction lists
			instrs = instruction_list_merge(instrs, ctx->instrs);

			i += 2;
		}
		else
		{
			// no operation (semicolon in the source code)
			i++;
		}
	}

	return instrs;
}

void analyse_assign(struct semantic_context *ctx, struct ast *assign)
{
	struct token id_tok = assign->children[0]->tok;
	
	if(id_tok.sym != NULL)
	{
		if(accept_variable(ctx, id_tok))
		{
			struct instruction *instr = ymalloc(sizeof(struct instruction));

			instr->type = INSTRUCTION_ASSIGN;
			instr->dest.type = OPERAND_SYMBOL;
			instr->dest.sym = id_tok.sym;
			instr->src1 = analyse_expression(ctx, assign->children[2]);
			instr->next = NULL;

			instruction_list_add(&ctx->instrs, instr);
		}
	}
}

void analyse_input(struct semantic_context *ctx, struct ast *input)
{
	struct token id_tok = input->children[1]->tok;

	if(id_tok.sym != NULL)
	{
		if(accept_variable(ctx, id_tok))
		{
			struct instruction *instr = ymalloc(sizeof(struct instruction));

			instr->type = INSTRUCTION_READ;
			instr->dest.type = OPERAND_SYMBOL;
			instr->dest.sym = id_tok.sym;
			instr->next = NULL;

			instruction_list_add(&ctx->instrs, instr);
		}
	}
}

void analyze_output(struct semantic_context *ctx, struct ast *output)
{
	struct instruction *instr = ymalloc(sizeof(struct instruction));

	instr->type = INSTRUCTION_WRITE;
	instr->src1 = analyse_expression(ctx, output->children[1]);

	instruction_list_add(&ctx->instrs, instr);
}

struct operand analyse_expression(struct semantic_context *ctx, struct ast *expression)
{
	struct operand opd1 = analyse_term(ctx, expression->children[0]);

	for(size_t i = 1; i < expression->children_cnt; i += 2)
	{
		struct operand opd2 = analyse_term(ctx, expression->children[i+1]);

		struct instruction *instr = ymalloc(sizeof(struct instruction));

		if(expression->children[i]->tok.type == TOKEN_PLUS)
			instr->type = INSTRUCTION_ADD;
		else
			instr->type = INSTRUCTION_SUB;

		instr->dest.type = OPERAND_TEMPORARY;
		instr->dest.tmp = ctx->tmp_cnt;
		instr->src1 = opd1;
		instr->src2 = opd2;
		instr->next = NULL;
		ctx->tmp_cnt++;

		instruction_list_add(&ctx->instrs, instr);

		opd1 = instr->dest;
	}

	return opd1;
}

struct operand analyse_term(struct semantic_context *ctx, struct ast *term)
{
	struct operand opd1 = analyse_factor(ctx, term->children[0]);

	for(size_t i = 1; i < term->children_cnt; i += 2)
	{
		struct operand opd2 = analyse_factor(ctx, term->children[i+1]);

		struct instruction *instr = ymalloc(sizeof(struct instruction));

		if(term->children[i]->tok.type == TOKEN_MUL)
			instr->type = INSTRUCTION_MUL;
		else
			instr->type = INSTRUCTION_DIV;

		instr->dest.type = OPERAND_TEMPORARY;
		instr->dest.tmp = ctx->tmp_cnt;
		instr->src1 = opd1;
		instr->src2 = opd2;
		instr->next = NULL;
		ctx->tmp_cnt++;

		instruction_list_add(&ctx->instrs, instr);

		opd1 = instr->dest;
	}

	return opd1;
}

struct operand analyse_factor(struct semantic_context *ctx, struct ast *factor)
{
	struct operand opd;
	struct instruction *instr;
	struct token tok = factor->children[0]->tok;

	switch(tok.type)
	{
		case TOKEN_LITERAL:
			opd.type = OPERAND_LITERAL;
			opd.lit = tok.lit;
			break;

		case TOKEN_IDENTIFIER:
			if(tok.sym != NULL)
			{
				if(accept_variable(ctx, tok))
				{
					opd.type = OPERAND_SYMBOL;
					opd.sym = tok.sym;
				}
			}
			break;

		case TOKEN_PLUS:
			instr = ymalloc(sizeof(struct instruction));
			instr->type = INSTRUCTION_PLS;
			instr->src1 = analyse_factor(ctx, factor->children[1]);
			instr->dest.type = OPERAND_TEMPORARY;
			instr->dest.tmp = ctx->tmp_cnt;
			instr->next = NULL;
			ctx->tmp_cnt++;
			instruction_list_add(&ctx->instrs, instr);
			opd = instr->dest;
			break;

		case TOKEN_MINUS:
			instr = ymalloc(sizeof(struct instruction));
			instr->type = INSTRUCTION_NEG;
			instr->src1 = analyse_factor(ctx, factor->children[1]);
			instr->dest.type = OPERAND_TEMPORARY;
			instr->dest.tmp = ctx->tmp_cnt;
			instr->next = NULL;
			ctx->tmp_cnt++;
			instruction_list_add(&ctx->instrs, instr);
			opd = instr->dest;
			break;

		case TOKEN_LPAREN:
			opd = analyse_expression(ctx, factor->children[1]);
			break;

		default:
			yassert(false, "invalid token");
			break;
	}

	return opd;
}

