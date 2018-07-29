
#include "instruction.h"

void instruction_list_init(struct instruction_list *instrs)
{
	instrs->head = NULL;
	instrs->tail = NULL;
}

void instruction_list_clear(struct instruction_list *instrs)
{
	struct instruction *tmp;

	while(instrs->head != NULL)
	{
		tmp = instrs->head;
		instrs->head = tmp->next;
		yfree(tmp);
	}

	instrs->tail = NULL;
}

bool instruction_list_empty(struct instruction_list instrs)
{
	return instrs.head == NULL;
}

void instruction_list_add(struct instruction_list *instrs, struct instruction *new_instr)
{
	if(instruction_list_empty(*instrs))
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

struct instruction_list instruction_list_merge(struct instruction_list instrs1, struct instruction_list instrs2)
{
	if(instruction_list_empty(instrs1))
	{
		return instrs2;
	}
	else if(instruction_list_empty(instrs2))
	{
		return instrs1;
	}
	else
	{
		struct instruction_list instrs;

		instrs.head = instrs1.head;
		instrs.tail = instrs2.tail;
		instrs1.tail->next = instrs2.head;

		return instrs;
	}
}

