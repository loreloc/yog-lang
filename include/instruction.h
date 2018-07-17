
/*! @file instruction.h */

#pragma once

#include "symtable.h"
#include "exprtree.h"

/*! @brief The types of an instruction node */
enum instr_type
{
	INSTR_ASSIGN,
	INSTR_INPUT,
	INSTR_OUTPUT
};

/*! @brief The node of the instruction list */
struct instr
{
	/*! @brief The type of the instruction node */
	enum instr_type type;

	/*! @brief A pointer to the symbol of the destination variable */
	struct symbol *sym;

	/*! @brief The expression tree to evaluate */
	struct expr_tree *tree;

	/*! @brief The next instruction node */
	struct instr *next;
};

/*! @brief The instruction list data structure */
struct instr_list
{
	/*! @brief The head of the instruction list */
	struct instr *head;

	/*! @brief The tail of the instruction list */
	struct instr *tail;
};

/**
 * @brief Make a new assign instruction
 * @param sym A pointer to the destination identifier symbol
 * @param tree A pointer to the source expression tree
 * @return A new instruction
 */
struct instr *instr_make_assign(struct symbol *sym, struct expr_tree *tree);

/**
 * @brief Make a new input instruction
 * @param sym A pointer to the identifier symbol to read
 * @return A new instruction
 */
struct instr *instr_make_input(struct symbol *sym);

/**
 * @brief Make a new output instruction
 * @param tree A pointer to the expression tree to evaluate and write the result
 * @return A new instruction
 */
struct instr *instr_make_output(struct expr_tree *tree);

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
 * @brief Check if an error list is empty
 * @param instrs The error list to check
 * @return true if the error list is empty, false otherwise
 */
bool instr_list_empty(struct instr_list instrs);

/**
 * @brief Add a new instruction node to an instruction list
 * @param instrs A pointer to the instruction list
 * @param new_instr A pointer to the new instruction node
 */
void instr_list_add(struct instr_list *instrs, struct instr *new_instr);

