
/*! @file interpreter.h */

#pragma once

#include "instructions.h"

/**
 * @brief The interpreter structure
 */
struct interpreter
{
	struct instr *ip; /*!< @brief The instruction pointer */
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

