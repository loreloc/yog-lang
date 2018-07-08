
#include "interpreter.h"

void interpreter_init(struct interpreter *vm, struct instr_list instrs)
{
	vm->ip = instrs.head;
}

void interpreter_execute(struct interpreter *vm)
{
	while(vm->ip)
	{
		struct instr ir = *vm->ip;

		// execute the instruction pointed by the instruction pointer
		switch(ir.type)
		{
			case INSTR_ASSIGN:
				ir.sym->value = expr_tree_eval(ir.tree);
				break;
			case INSTR_INPUT:
				printf("enter the value of %s : ", ir.sym->id);
				scanf("%ld", &ir.sym->value);
				break;
			default: // case INSTR_OUTPUT:
				printf("%ld\n", expr_tree_eval(ir.tree));
				break;
		}

		vm->ip = vm->ip->next;
	}
}

