
#include "interpreter.h"

int64_t operand_get_value(struct interpreter *vm, struct operand op)
{
	switch(op.type)
	{
		case OPERAND_TEMPORARY:
			return vm->temporary[op.index];
		
		case OPERAND_LITERAL:
			return op.lit;

		case OPERAND_SYMBOL:
			return op.sym->value;

		default: // case OPERAND_LABEL:
			return op.index;
	}
}

void interpreter_init(struct interpreter *vm, struct instruction_list instrs, size_t tmp_cnt)
{
	vm->temporary = ymalloc(tmp_cnt * sizeof(int64_t));
	vm->instrs = instrs;
	vm->pc = 0;
}

void interpreter_clear(struct interpreter *vm)
{
	yfree(vm->temporary);
	vm->temporary = NULL;
	vm->pc = 0;
}

void interpreter_execute(struct interpreter *vm)
{
	while(vm->pc < vm->instrs.size)
	{
		struct instruction instr = vm->instrs.data[vm->pc];

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
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) + operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_SUB:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) - operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_MUL:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) * operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_DIV:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) / operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_PLS:
				vm->temporary[instr.dest.index] = +operand_get_value(vm, instr.src1);
				break;

			case INSTRUCTION_NEG:
				vm->temporary[instr.dest.index] = -operand_get_value(vm, instr.src1);
				break;

			case INSTRUCTION_EQ:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) == operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_NEQ:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) != operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_LT:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) < operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_LTE:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) <= operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_GT:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) > operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_GTE:
				vm->temporary[instr.dest.index] = operand_get_value(vm, instr.src1) >= operand_get_value(vm, instr.src2);
				break;

			case INSTRUCTION_GOTO:
				vm->pc = instr.dest.index;
				continue;
				break;

			case INSTRUCTION_BRANCH:
				if(vm->temporary[instr.src1.index])
				{
					vm->pc = instr.dest.index;
					continue;
				}
				break;

			default:
				yassert(false, "unknow instruction");
				break;
		}

		vm->pc++;
	}
}

