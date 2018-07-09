
/*! @file interpreter.h */

#pragma once

#include "instruction.h"

/*! @brief The interpreter data structure */
struct interpreter
{
	/*! @brief The instruction pointer */
	struct instr *ip;
};

/**
 * @brief Initialzie an interpreter
 * @param vm A pointer to the interpreter
 * @param instrs The instruction list to bind
 */
void interpreter_init(struct interpreter *vm, struct instr_list instrs);

/**
 * @brief Execute the interpreter
 * @param vm A pointer to the interpreter
 */
void interpreter_execute(struct interpreter *vm);

