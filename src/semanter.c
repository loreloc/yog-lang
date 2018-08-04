
#include "semanter.h"

size_t next_temporary(struct semantic_context *ctx);
bool accept_variable(struct semantic_context *ctx, struct token tok);
size_t push_label(struct semantic_context *ctx, struct instruction **instr);

void analyse_variables(struct semantic_context *ctx, struct ast *variables);
struct instruction_list analyse_statements(struct semantic_context *ctx, struct ast *statements);

struct instruction_list analyse_assign(struct semantic_context *ctx, struct ast *assign);
struct instruction_list analyse_input(struct semantic_context *ctx, struct ast *input);
struct instruction_list analyse_output(struct semantic_context *ctx, struct ast *output);
struct instruction_list analyse_branch(struct semantic_context *ctx, struct ast *branch);

struct operand analyse_expression(struct semantic_context *ctx, struct ast *expression);
struct operand analyse_term(struct semantic_context *ctx, struct ast *term);
struct operand analyse_factor(struct semantic_context *ctx, struct ast *factor);
struct operand analyse_condition(struct semantic_context *ctx, struct ast *condition);

void semantic_context_init(struct semantic_context *ctx, struct symbol_table *st, struct error_list *errs, struct ast *tree)
{
	ctx->st = st;
	ctx->errs = errs;
	ctx->tree = tree;

	ctx->instrs.head = NULL;
	ctx->instrs.tail = NULL;

	ctx->labels_cnt = 0;
	ctx->labels = NULL;

	ctx->tmp_cnt = 0;
}

struct instruction_list semantic_context_analyse(struct semantic_context *ctx)
{
	// analyse the variables and update the symbol table
	analyse_variables(ctx, ctx->tree->children[1]);

	// analyse the statements and produce the instructio nlist
	return analyse_statements(ctx, ctx->tree->children[3]);
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

size_t push_label(struct semantic_context *ctx, struct instruction **instr)
{
	ctx->labels = yrealloc(ctx->labels, (ctx->labels_cnt + 1) * sizeof(label_t));
	ctx->labels[ctx->labels_cnt] = instr;

	return ctx->labels_cnt++;
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

struct instruction_list analyse_statements(struct semantic_context *ctx, struct ast *statements)
{
	struct instruction_list instrs;
	instruction_list_init(&instrs);

	for(size_t i = 0; i < statements->children_cnt; i++)
	{
		struct ast *stmt = statements->children[i];

		switch(stmt->nt)
		{
			case AST_NT_ASSIGN:
				instrs = instruction_list_merge(instrs, analyse_assign(ctx, stmt));
				break;

			case AST_NT_INPUT:
				instrs = instruction_list_merge(instrs, analyse_input(ctx, stmt));
				break;

			case AST_NT_OUTPUT:
				instrs = instruction_list_merge(instrs, analyse_output(ctx, stmt));
				break;

			case AST_NT_BRANCH:
				instrs = instruction_list_merge(instrs, analyse_branch(ctx, stmt));
				break;

			default:
				yassert(false, "unknow nonterminal");
				break;
		}

		instruction_list_init(&ctx->instrs);
	}

	return instrs;
}

struct instruction_list analyse_assign(struct semantic_context *ctx, struct ast *assign)
{
	struct instruction_list instrs;
	instruction_list_init(&instrs);

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

			instrs = ctx->instrs;
			instruction_list_add(&instrs, instr);
		}
	}

	return instrs;
}

struct instruction_list analyse_input(struct semantic_context *ctx, struct ast *input)
{
	struct instruction_list instrs;
	instruction_list_init(&instrs);

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

			instruction_list_add(&instrs, instr);
		}
	}

	return instrs;
}

struct instruction_list analyse_output(struct semantic_context *ctx, struct ast *output)
{
	struct instruction_list instrs;
	instruction_list_init(&instrs);

	struct instruction *instr = ymalloc(sizeof(struct instruction));

	instr->type = INSTRUCTION_WRITE;
	instr->src1 = analyse_expression(ctx, output->children[1]);
	instr->next = NULL;

	instrs = ctx->instrs;
	instruction_list_add(&instrs, instr);

	return instrs;
}

struct instruction_list analyse_branch(struct semantic_context *ctx, struct ast *branch)
{
	struct instruction_list instrs;
	instruction_list_init(&instrs);

	struct instruction_list if_branch   = analyse_statements(ctx, branch->children[5]);
	struct instruction_list else_branch = analyse_statements(ctx, branch->children[7]);

	struct instruction *goto_instr = ymalloc(sizeof(struct instruction));
	goto_instr->type = INSTRUCTION_GOTO;
	goto_instr->dest.type = OPERAND_LABEL;

	if(instruction_list_empty(if_branch))
		goto_instr->dest.index = push_label(ctx, &goto_instr->next);
	else
		goto_instr->dest.index = push_label(ctx, &if_branch.tail->next);

	goto_instr->next = NULL;

	struct instruction *branch_instr = ymalloc(sizeof(struct instruction));
	branch_instr->type = INSTRUCTION_BRANCH;
	branch_instr->src1 = analyse_condition(ctx, branch->children[2]);
	branch_instr->dest.type = OPERAND_LABEL;
	branch_instr->dest.index = push_label(ctx, &goto_instr->next);
	branch_instr->next = NULL;

	instrs = ctx->instrs;
	instruction_list_add(&instrs, branch_instr);
	instrs = instruction_list_merge(instrs, else_branch);
	instruction_list_add(&instrs, goto_instr);
	instrs = instruction_list_merge(instrs, if_branch);

	return instrs;
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
		instr->dest.index = next_temporary(ctx);
		instr->src1 = opd1;
		instr->src2 = opd2;
		instr->next = NULL;

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
		instr->dest.index = next_temporary(ctx);
		instr->src1 = opd1;
		instr->src2 = opd2;
		instr->next = NULL;

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
			instr->dest.index = next_temporary(ctx);
			instr->next = NULL;
			instruction_list_add(&ctx->instrs, instr);
			opd = instr->dest;
			break;

		case TOKEN_MINUS:
			instr = ymalloc(sizeof(struct instruction));
			instr->type = INSTRUCTION_NEG;
			instr->src1 = analyse_factor(ctx, factor->children[1]);
			instr->dest.type = OPERAND_TEMPORARY;
			instr->dest.index = next_temporary(ctx);
			instr->next = NULL;
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

struct operand analyse_condition(struct semantic_context *ctx, struct ast *condition)
{
	struct instruction *instr = ymalloc(sizeof(struct instruction));

	instr->src1 = analyse_expression(ctx, condition->children[0]);
	instr->src2 = analyse_expression(ctx, condition->children[2]);
	instr->dest.type = OPERAND_TEMPORARY;
	instr->dest.index = next_temporary(ctx);
	instr->next = NULL;

	switch(condition->children[1]->tok.type)
	{
		case TOKEN_EQ:
			instr->type = INSTRUCTION_EQ;
			break;

		case TOKEN_NEQ:
			instr->type = INSTRUCTION_NEQ;
			break;

		case TOKEN_LT:
			instr->type = INSTRUCTION_LT;
			break;

		case TOKEN_LTE:
			instr->type = INSTRUCTION_LTE;
			break;

		case TOKEN_GT:
			instr->type = INSTRUCTION_GT;
			break;

		case TOKEN_GTE:
			instr->type = INSTRUCTION_GTE;
			break;

		default:
			yassert(false, "invalid token");
			break;
	}

	// add the condition to the instruction list
	instruction_list_add(&ctx->instrs, instr);

	return instr->dest;
}

