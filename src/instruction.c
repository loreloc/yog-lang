
#include "instruction.h"

struct instr *instr_make_assign(struct symbol *sym, struct expr_tree *tree)
{
	struct instr *new_instr = ymalloc(sizeof(struct instr));

	new_instr->next = NULL;
	new_instr->type = INSTR_ASSIGN;
	new_instr->sym = sym;
	new_instr->tree = tree;

	return new_instr;
}

struct instr *instr_make_input(struct symbol *sym)
{
	struct instr *new_instr = ymalloc(sizeof(struct instr));

	new_instr->next = NULL;
	new_instr->type = INSTR_INPUT;
	new_instr->sym = sym;

	return new_instr;
}

struct instr *instr_make_output(struct expr_tree *tree)
{
	struct instr *new_instr = ymalloc(sizeof(struct instr));

	new_instr->next = NULL;
	new_instr->type = INSTR_OUTPUT;
	new_instr->tree = tree;

	return new_instr;
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
		free(tmp);
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

