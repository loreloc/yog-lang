
#include "instruction.h"

struct instr *instr_make_assign(struct symbol *sym, struct expr_tree *tree)
{
	struct instr *instr = ymalloc(sizeof(struct instr));

	instr->next = NULL;
	instr->type = INSTR_ASSIGN;
	instr->sym = sym;
	instr->tree = tree;

	return instr;
}

struct instr *instr_make_input(struct symbol *sym)
{
	struct instr *instr = ymalloc(sizeof(struct instr));

	instr->next = NULL;
	instr->type = INSTR_INPUT;
	instr->sym = sym;

	return instr;
}

struct instr *instr_make_output(struct expr_tree *tree)
{
	struct instr *instr = ymalloc(sizeof(struct instr));

	instr->next = NULL;
	instr->type = INSTR_OUTPUT;
	instr->tree = tree;

	return instr;
}

void instr_list_init(struct instr_list *instrs)
{
	instrs->head = NULL;
	instrs->tail = NULL;
}

void instr_list_clear(struct instr_list *instrs)
{
	struct instr *tmp;

	while(instrs->head != NULL)
	{
		tmp = instrs->head;

		if(tmp->type == INSTR_ASSIGN || tmp->type == INSTR_OUTPUT)
			expr_tree_clear(tmp->tree);

		instrs->head = tmp->next;
		yfree(tmp);
	}

	instrs->tail = NULL;
}

bool instr_list_empty(struct instr_list instrs)
{
	return instrs.head == NULL;
}

void instr_list_add(struct instr_list *instrs, struct instr *new_instr)
{
	if(instr_list_empty(*instrs))
	{
		instrs->head = new_instr;
		instrs->tail = new_instr;
	}
	else
	{
		instrs->tail->next = new_instr;
		instrs->tail = new_instr;
	}
}

