
#include "instruction.h"

void instruction_list_init(struct instruction_list *instrs)
{
	instrs->data = NULL;
	instrs->size = 0;
	instrs->capacity = 0;
}

void instruction_list_clear(struct instruction_list *instrs)
{
	yfree(instrs->data);
	instrs->data = NULL;
	instrs->size = 0;
	instrs->capacity = 0;
}

bool instruction_list_empty(struct instruction_list instrs)
{
	return instrs.size == 0;
}

void instruction_list_add(struct instruction_list *instrs, struct instruction new_instr)
{
	if(instrs->size >= instrs->capacity)
	{
		instrs->capacity += 8;
		instrs->data = yrealloc(instrs->data, instrs->capacity * sizeof(struct instruction));
	}

	instrs->data[instrs->size++] = new_instr;
}

