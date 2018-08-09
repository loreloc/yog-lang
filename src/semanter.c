
#include "semanter.h"

size_t next_temporary(struct semantic_context *ctx);
bool accept_variable(struct semantic_context *ctx, struct token tok);

void analyse_variables(struct semantic_context *ctx, struct ast *variables);
void analyse_statements(struct semantic_context *ctx, struct ast *statements);

void analyse_assign(struct semantic_context *ctx, struct ast *assign);
void analyse_input(struct semantic_context *ctx, struct ast *input);
void analyse_output(struct semantic_context *ctx, struct ast *output);
void analyse_branch(struct semantic_context *ctx, struct ast *branch);
void analyse_loop(struct semantic_context *ctx, struct ast *loop);

struct operand analyse_expression(struct semantic_context *ctx, struct ast *expression);
struct operand analyse_term(struct semantic_context *ctx, struct ast *term);
struct operand analyse_factor(struct semantic_context *ctx, struct ast *factor);
struct operand analyse_condition(struct semantic_context *ctx, struct ast *condition);

void semantic_context_init(struct semantic_context *ctx, struct symbol_table *st, struct error_list *errs, struct ast *tree)
{
	ctx->st = st;
	ctx->errs = errs;
	ctx->tree = tree;

	instruction_list_init(&ctx->instrs);

	ctx->tmp_cnt = 0;
}

struct instruction_list semantic_context_analyse(struct semantic_context *ctx)
{
	analyse_variables(ctx, ctx->tree->children[1]);
	analyse_statements(ctx, ctx->tree->children[3]);

	return ctx->instrs;
}

size_t next_temporary(struct semantic_context *ctx)
{
	return ctx->tmp_cnt++;
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

void analyse_variables(struct semantic_context *ctx, struct ast *variables)
{
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

void analyse_statements(struct semantic_context *ctx, struct ast *statements)
{
	for(size_t i = 0; i < statements->children_cnt; i++)
	{
		struct ast *stmt = statements->children[i];

		switch(stmt->nt)
		{
			case AST_NT_ASSIGN:
				analyse_assign(ctx, stmt);
				break;

			case AST_NT_INPUT:
				analyse_input(ctx, stmt);
				break;

			case AST_NT_OUTPUT:
				analyse_output(ctx, stmt);
				break;

			case AST_NT_BRANCH:
				analyse_branch(ctx, stmt);
				break;

			case AST_NT_LOOP:
				analyse_loop(ctx, stmt);
				break;

			default:
				yassert(false, "unknow nonterminal");
				break;
		}
	}
}

void analyse_assign(struct semantic_context *ctx, struct ast *assign)
{
	struct token id_tok = assign->children[0]->tok;
	
	if(id_tok.sym != NULL)
	{
		if(accept_variable(ctx, id_tok))
		{
			struct instruction instr;
			instr.type = INSTRUCTION_ASSIGN;
			instr.dest.type = OPERAND_SYMBOL;
			instr.dest.sym = id_tok.sym;
			instr.src1 = analyse_expression(ctx, assign->children[2]);

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
			struct instruction instr;
			instr.type = INSTRUCTION_READ;
			instr.dest.type = OPERAND_SYMBOL;
			instr.dest.sym = id_tok.sym;

			instruction_list_add(&ctx->instrs, instr);
		}
	}
}

void analyse_output(struct semantic_context *ctx, struct ast *output)
{
	struct instruction instr;
	instr.type = INSTRUCTION_WRITE;
	instr.src1 = analyse_expression(ctx, output->children[1]);

	instruction_list_add(&ctx->instrs, instr);
}

void analyse_branch(struct semantic_context *ctx, struct ast *branch)
{
	struct instruction branch_instr;
	branch_instr.type = INSTRUCTION_BRANCH;
	branch_instr.src1 = analyse_condition(ctx, branch->children[2]);
	branch_instr.dest.type = OPERAND_LABEL;
	instruction_list_add(&ctx->instrs, branch_instr);

	struct instruction *branch_instr_ptr = &ctx->instrs.data[ctx->instrs.size - 1];

	analyse_statements(ctx, branch->children[7]);

	struct instruction goto_instr;
	goto_instr.type = INSTRUCTION_GOTO;
	goto_instr.dest.type = OPERAND_LABEL;
	instruction_list_add(&ctx->instrs, goto_instr);

	struct instruction *goto_instr_ptr = &ctx->instrs.data[ctx->instrs.size - 1];

	branch_instr_ptr->dest.index = ctx->instrs.size;

	analyse_statements(ctx, branch->children[5]);

	goto_instr_ptr->dest.index = ctx->instrs.size;
}

void analyse_loop(struct semantic_context *ctx, struct ast *loop)
{
	size_t start_label = ctx->instrs.size;

	struct instruction branch_instr;
	branch_instr.type = INSTRUCTION_BRANCH;
	branch_instr.src1 = analyse_condition(ctx, loop->children[2]);
	branch_instr.dest.type = OPERAND_LABEL;
	instruction_list_add(&ctx->instrs, branch_instr);

	struct instruction *branch_instr_ptr = &ctx->instrs.data[ctx->instrs.size - 1];

	struct instruction exit_instr;
	exit_instr.type = INSTRUCTION_GOTO;
	exit_instr.dest.type = OPERAND_LABEL;
	instruction_list_add(&ctx->instrs, exit_instr);

	struct instruction *exit_instr_ptr = &ctx->instrs.data[ctx->instrs.size - 1];

	branch_instr_ptr->dest.index = ctx->instrs.size;

	analyse_statements(ctx, loop->children[5]);

	struct instruction goto_instr;
	goto_instr.type = INSTRUCTION_GOTO;
	goto_instr.dest.type = OPERAND_LABEL;
	goto_instr.dest.index = start_label;
	instruction_list_add(&ctx->instrs, goto_instr);

	exit_instr_ptr->dest.index = ctx->instrs.size;
}

struct operand analyse_expression(struct semantic_context *ctx, struct ast *expression)
{
	struct operand opd1 = analyse_term(ctx, expression->children[0]);

	for(size_t i = 1; i < expression->children_cnt; i += 2)
	{
		struct operand opd2 = analyse_term(ctx, expression->children[i+1]);

		struct instruction instr;

		if(expression->children[i]->tok.type == TOKEN_PLUS)
			instr.type = INSTRUCTION_ADD;
		else
			instr.type = INSTRUCTION_SUB;

		instr.dest.type = OPERAND_TEMPORARY;
		instr.dest.index = next_temporary(ctx);
		instr.src1 = opd1;
		instr.src2 = opd2;

		instruction_list_add(&ctx->instrs, instr);

		opd1 = instr.dest;
	}

	return opd1;
}

struct operand analyse_term(struct semantic_context *ctx, struct ast *term)
{
	struct operand opd1 = analyse_factor(ctx, term->children[0]);

	for(size_t i = 1; i < term->children_cnt; i += 2)
	{
		struct operand opd2 = analyse_factor(ctx, term->children[i+1]);

		struct instruction instr;

		if(term->children[i]->tok.type == TOKEN_MUL)
			instr.type = INSTRUCTION_MUL;
		else
			instr.type = INSTRUCTION_DIV;

		instr.dest.type = OPERAND_TEMPORARY;
		instr.dest.index = next_temporary(ctx);
		instr.src1 = opd1;
		instr.src2 = opd2;

		instruction_list_add(&ctx->instrs, instr);

		opd1 = instr.dest;
	}

	return opd1;
}

struct operand analyse_factor(struct semantic_context *ctx, struct ast *factor)
{
	struct operand opd;
	struct instruction instr;

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
			instr.type = INSTRUCTION_PLS;
			instr.src1 = analyse_factor(ctx, factor->children[1]);
			instr.dest.type = OPERAND_TEMPORARY;
			instr.dest.index = next_temporary(ctx);
			instruction_list_add(&ctx->instrs, instr);
			opd = instr.dest;
			break;

		case TOKEN_MINUS:
			instr.type = INSTRUCTION_NEG;
			instr.src1 = analyse_factor(ctx, factor->children[1]);
			instr.dest.type = OPERAND_TEMPORARY;
			instr.dest.index = next_temporary(ctx);
			instruction_list_add(&ctx->instrs, instr);
			opd = instr.dest;
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

struct operand analyse_condition(struct semantic_context *ctx, struct ast *condition)
{
	struct instruction instr;
	instr.src1 = analyse_expression(ctx, condition->children[0]);
	instr.src2 = analyse_expression(ctx, condition->children[2]);
	instr.dest.type = OPERAND_TEMPORARY;
	instr.dest.index = next_temporary(ctx);

	switch(condition->children[1]->tok.type)
	{
		case TOKEN_EQ:
			instr.type = INSTRUCTION_EQ;
			break;

		case TOKEN_NEQ:
			instr.type = INSTRUCTION_NEQ;
			break;

		case TOKEN_LT:
			instr.type = INSTRUCTION_LT;
			break;

		case TOKEN_LTE:
			instr.type = INSTRUCTION_LTE;
			break;

		case TOKEN_GT:
			instr.type = INSTRUCTION_GT;
			break;

		case TOKEN_GTE:
			instr.type = INSTRUCTION_GTE;
			break;

		default:
			yassert(false, "invalid token");
			break;
	}

	instruction_list_add(&ctx->instrs, instr);

	return instr.dest;
}

