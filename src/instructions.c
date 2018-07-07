
#include "instructions.h"

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
		instrs->head = tmp->next;
		free(tmp);
	}

	instrs->tail = NULL;
}

void instr_list_add_assignment(struct instr_list *instrs, struct symbol *sym, struct expr_tree *tree)
{
	// allocate a new instruction node
	struct instr *new_instr = ymalloc(sizeof(new_instr));
	new_instr->next = NULL;
	new_instr->type = INSTR_ASSIGNMENT;
	new_instr->sym = sym;
	new_instr->tree = tree;

	// add a new instruction node to the instruction list
	if(instrs->head)
	{
		instrs->tail->next = new_instr;
		instrs->tail = new_instr;
	}
	else
	{
		instrs->head = new_instr;
		instrs->tail = new_instr;
	}
}

void instr_list_add_input(struct instr_list *instrs, struct symbol *sym)
{
	// allocate a new instruction node
	struct instr *new_instr = ymalloc(sizeof(new_instr));
	new_instr->next = NULL;
	new_instr->type = INSTR_INPUT;
	new_instr->sym = sym;

	// add a new instruction node to the instruction list
	if(instrs->head)
	{
		instrs->tail->next = new_instr;
		instrs->tail = new_instr;
	}
	else
	{
		instrs->head = new_instr;
		instrs->tail = new_instr;
	}
}

void instr_list_add_output(struct instr_list *instrs, struct expr_tree *tree)
{
	// allocate a new instruction node
	struct instr *new_instr = ymalloc(sizeof(new_instr));
	new_instr->next = NULL;
	new_instr->type = INSTR_OUTPUT;
	new_instr->tree = tree;

	// add a new instruction node to the instruction list
	if(instrs->head)
	{
		instrs->tail->next = new_instr;
		instrs->tail = new_instr;
	}
	else
	{
		instrs->head = new_instr;
		instrs->tail = new_instr;
	}
}

