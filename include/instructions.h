
/*! @file instructions.h */

#pragma once

#include "symtable.h"
#include "exprtree.h"

/**
 * @brief The possible types of an instruction
 */
enum instr_type
{
	INSTR_ASSIGNMENT,
	INSTR_INPUT,
	INSTR_OUTPUT
};

/**
 * @brief The node of an instruction list
 */
struct instr
{
	enum instr_type type; /*!< @brief The type of the instruction */

	union
	{
		struct symbol *sym;     /*!< @brief The symbol field */
		struct expr_tree *tree; /*!< @brief The expression tree field */
	};

	struct instr *next; /*!< @brief The next instruction */
};

/**
 * @brief The instruction list
 */
struct instr_list
{
	struct instr *head; /*!< @brief The head of the instruction list */
	struct instr *tail; /*!< @brief The tail of the instruction list */
};

/**
 * @brief Initialize an instruction list
 * @param instrs A pointer to the instruction list to initialize
 */
void instr_list_init(struct instr_list *instrs);

/**
 * @brief Clear an instruction list
 * @param instrs A pointer to the instruction list to clear
 */
void instr_list_clear(struct instr_list *instrs);

/**
 * @brief Add a new assignment instruction to an instruction list
 * @param instrs A pointer to the instructions list
 * @param sym A pointer to the destination identifier symbol
 * @param tree A pointer to the source expression tree
 */
void instr_list_add_assignment(struct instr_list *instrs, struct symbol *sym, struct expr_tree *tree);

/**
 * @brief Add a new input instruction to an instruction list
 * @param instrs A pointer to the instructions list
 * @param sym A pointer to the identifier symbol to read
 */
void instr_list_add_input(struct instr_list *instrs, struct symbol *sym);

/**
 * @brief Add a new output instruction to an instruction list
 * @param instrs A pointer to the instructions list
 * @param tree A pointer to the expression tree to write
 */
void instr_list_add_output(struct instr_list *instrs, struct expr_tree *tree);

