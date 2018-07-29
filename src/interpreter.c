
#include "interpreter.h"

int64_t operand_get_value(struct interpreter *vm, struct operand op)
{
	switch(op.type)
	{
		case OPERAND_TEMPORARY:
			return vm->temporary[op.tmp];
		
		case OPERAND_LITERAL:
			return op.lit;

		default: // case OPERAND_SYMBOL:
			return op.sym->value;
	}
}

void interpreter_init(struct interpreter *vm, struct instruction_list instrs, size_t tmp_cnt)
{
	// allocate the temporary variables buffer
	vm->temporary = ymalloc(tmp_cnt * sizeof(int64_t));

	// initiapize the instruction pointer
	vm->ip = instrs.head;
}

void interpreter_clear(struct interpreter *vm)
{
	yfree(vm->temporary);
	vm->temporary = NULL;

	vm->ip = NULL;
}

void interpreter_execute(struct interpreter *vm)
{
	while(vm->ip)
	{
		struct instruction instr = *vm->ip;

		// execute the instruction pointed by the instruction pointer
		switch(instr.type)
		{
			case INSTRUCTION_ASSIGN:
				instr.dest.sym->value = operand_get_value(vm, instr.src1);
				break;

			case INSTRUCTION_READ:
				printf("enter the value of \"%s\": ", instr.dest.sym->id);
				scanf("%ld", &instr.dest.sym->value);
				break;

			case INSTRUCTION_WRITE:
				printf("%ld\n", operand_get_value(vm, instr.src1));
				break;

			case INSTRUCTION_ADD:
				vm->temporary[instr.dest.tmp] = operand_get_value(vm, instr.src1) + operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_SUB:
				vm->temporary[instr.dest.tmp] = operand_get_value(vm, instr.src1) - operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_MUL:
				vm->temporary[instr.dest.tmp] = operand_get_value(vm, instr.src1) * operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_DIV:
				vm->temporary[instr.dest.tmp] = operand_get_value(vm, instr.src1) / operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_PLS:
				vm->temporary[instr.dest.tmp] = +operand_get_value(vm, instr.src1);
				break;

			case INSTRUCTION_NEG:
				vm->temporary[instr.dest.tmp] = -operand_get_value(vm, instr.src1);
				break;

			default:
				yassert(false, "unknow instruction");
				break;
		}

		vm->ip = vm->ip->next;
	}
}

