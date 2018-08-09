
#include "interpreter.h"

typedef void (* function_t) (struct interpreter *vm, struct instruction instr);

void execute_assign(struct interpreter *vm, struct instruction instr);
void execute_read(struct interpreter *vm, struct instruction instr);
void execute_write(struct interpreter *vm, struct instruction instr);
void execute_add(struct interpreter *vm, struct instruction instr);
void execute_sub(struct interpreter *vm, struct instruction instr);
void execute_mul(struct interpreter *vm, struct instruction instr);
void execute_div(struct interpreter *vm, struct instruction instr);
void execute_pls(struct interpreter *vm, struct instruction instr);
void execute_neg(struct interpreter *vm, struct instruction instr);
void execute_eq(struct interpreter *vm, struct instruction instr);
void execute_neq(struct interpreter *vm, struct instruction instr);
void execute_lt(struct interpreter *vm, struct instruction instr);
void execute_lte(struct interpreter *vm, struct instruction instr);
void execute_gt(struct interpreter *vm, struct instruction instr);
void execute_gte(struct interpreter *vm, struct instruction instr);
void execute_goto(struct interpreter *vm, struct instruction instr);
void execute_branch(struct interpreter *vm, struct instruction instr);

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
	static const function_t Function_Table[17] =
	{
		execute_assign,
		execute_read,
		execute_write,
		execute_add,
		execute_sub,
		execute_mul,
		execute_div,
		execute_pls,
		execute_neg,
		execute_eq,
		execute_neq,
		execute_lt,
		execute_lte,
		execute_gt,
		execute_gte,
		execute_goto,
		execute_branch
	};

	while(vm->pc < vm->instrs.size)
	{
		// get the instruction pointed by the program counter
		struct instruction instr = vm->instrs.data[vm->pc];

		// execute the instruction
		Function_Table[instr.type](vm, instr);
	}
}

void execute_assign(struct interpreter *vm, struct instruction instr)
{
	instr.dest.sym->value = operand_get_value(vm, instr.src1);
	vm->pc++;
}

void execute_read(struct interpreter *vm, struct instruction instr)
{
	printf("enter the value of \"%s\": ", instr.dest.sym->id);
	scanf("%ld", &instr.dest.sym->value);
	vm->pc++;
}

void execute_write(struct interpreter *vm, struct instruction instr)
{
	printf("%ld\n", operand_get_value(vm, instr.src1));
	vm->pc++;
}

void execute_add(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left + right;
	vm->pc++;
}

void execute_sub(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left - right;
	vm->pc++;
}

void execute_mul(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left * right;
	vm->pc++;
}

void execute_div(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	yassert(right != 0, "division by zero");
	vm->temporary[instr.dest.index] = left / right;
	vm->pc++;
}

void execute_pls(struct interpreter *vm, struct instruction instr)
{
	vm->temporary[instr.dest.index] = +operand_get_value(vm, instr.src1);
	vm->pc++;
}

void execute_neg(struct interpreter *vm, struct instruction instr)
{
	vm->temporary[instr.dest.index] = -operand_get_value(vm, instr.src1);
	vm->pc++;
}

void execute_eq(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left == right;
	vm->pc++;
}

void execute_neq(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left != right;
	vm->pc++;
}

void execute_lt(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left < right;
	vm->pc++;
}

void execute_lte(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left <= right;
	vm->pc++;
}

void execute_gt(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left > right;
	vm->pc++;
}

void execute_gte(struct interpreter *vm, struct instruction instr)
{
	int64_t left  = operand_get_value(vm, instr.src1);
	int64_t right = operand_get_value(vm, instr.src2);
	vm->temporary[instr.dest.index] = left >= right;
	vm->pc++;
}

void execute_goto(struct interpreter *vm, struct instruction instr)
{
	vm->pc = instr.dest.index;
}

void execute_branch(struct interpreter *vm, struct instruction instr)
{
	if(vm->temporary[instr.src1.index])
		vm->pc = instr.dest.index;
	else
		vm->pc++;
}

