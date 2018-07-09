
#include "instruction.h"

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

void instr_list_add(struct instr_list *instrs, struct instr *new_node)
{
	if(instr_list_empty(*instrs))
	{
		instrs->head = new_node;
		instrs->tail = new_node;
	}
	else
	{
		instrs->tail->next = new_node;
		instrs->tail = new_node;
	}
}

void instr_list_add_assign(struct instr_list *instrs, struct symbol *sym, struct expr_tree *tree)
{
	struct instr *new_node = ymalloc(sizeof(struct instr));

	new_node->next = NULL;
	new_node->type = INSTR_ASSIGN;
	new_node->sym = sym;
	new_node->tree = tree;

	instr_list_add(instrs, new_node);
}

void instr_list_add_input(struct instr_list *instrs, struct symbol *sym)
{
	struct instr *new_node = ymalloc(sizeof(struct instr));

	new_node->next = NULL;
	new_node->type = INSTR_INPUT;
	new_node->sym = sym;

	instr_list_add(instrs, new_node);
}

void instr_list_add_output(struct instr_list *instrs, struct expr_tree *tree)
{
	struct instr *new_node = ymalloc(sizeof(struct instr));

	new_node->next = NULL;
	new_node->type = INSTR_OUTPUT;
	new_node->tree = tree;

	instr_list_add(instrs, new_node);
}

