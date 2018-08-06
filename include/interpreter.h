
/*! @file interpreter.h */

#pragma once

#include "instruction.h"

/*! @brief The interpreter data structure */
struct interpreter
{
	/*! @brief The buffer of temporary variables */
	int64_t *temporary;

	/*! @brief The instruction list */
	struct instruction_list instrs;

	/*! @brief The program counter */
	size_t pc;
};

/**
 * @brief Get the value of an operand
 * @param vm A pointer to the interpreter
 * @param op The operand
 * @return The value of the operand
 */
int64_t operand_get_value(struct interpreter *vm, struct operand op);

/**
 * @brief Initialzie an interpreter
 * @param vm A pointer to the interpreter
 * @param instrs The instruction list to bind

 * @param tmp_cnt The number of temporary variables
 */
void interpreter_init(struct interpreter *vm, struct instruction_list instrs, size_t tmp_cnt);

/**
 * @brief Clear an interpreter
 * @param vm A pointer to the interpreter to clear
 */
void interpreter_clear(struct interpreter *vm);

/**
 * @brief Execute the interpreter
 * @param vm A pointer to the interpreter
 */
void interpreter_execute(struct interpreter *vm);

