
/*! @file instruction.h */

#pragma once

#include "symtable.h"

/*! @brief The types of a variable */
enum operand_type
{
	OPERAND_TEMPORARY,
	OPERAND_LITERAL,
	OPERAND_SYMBOL,
	OPERAND_LABEL,
};

/*! @brief The operand data structure */
struct operand
{
	/*! @brief The type of the variable */
	enum operand_type type;

	union
	{
		/*! @brief The index of the temporary variable or the index of a label */
		size_t index;

		/*! @brief The literal value */
		int64_t lit;

		/*! @brief A pointer to the symbol value of the variable */
		struct symbol *sym;
	};
};

/*! @brief The types of an instruction */
enum instruction_type
{
	INSTRUCTION_ASSIGN,
	INSTRUCTION_READ,
	INSTRUCTION_WRITE,
	INSTRUCTION_ADD,
	INSTRUCTION_SUB,
	INSTRUCTION_MUL,
	INSTRUCTION_DIV,
	INSTRUCTION_PLS,
	INSTRUCTION_NEG,
	INSTRUCTION_EQ,
	INSTRUCTION_NEQ,
	INSTRUCTION_LT,
	INSTRUCTION_LTE,
	INSTRUCTION_GT,
	INSTRUCTION_GTE,
	INSTRUCTION_GOTO,
	INSTRUCTION_BRANCH
};

/*! @brief The three address code instruction */
struct instruction
{
	/*! @brief The type of the instruction */
	enum instruction_type type;

	/*! @brief The first operand */
	struct operand src1;

	/*! @brief The second operand */
	struct operand src2;

	/*! @brief The destination operand */
	struct operand dest;

	/*! @brief The next instruction */
	struct instruction *next;
};

/*! @brief The instruction list */
struct instruction_list
{
	/*! @brief The head of the list */
	struct instruction *head;

	/*! @brief The tail of the list */
	struct instruction *tail;
};

/**
 * @brief Initialize an instruction list
 * @param instrs A pointer to the instruction list to initialize
 */
void instruction_list_init(struct instruction_list *instrs);

/**
 * @brief Clear an instruction list
 * @param instrs A pointer to the instruction list co clear
 */
void instruction_list_clear(struct instruction_list *instrs);

/**
 * @brief Check if an instruction list is empty
 * @param instrs The instruction list to check
 * @return ture if the instruction list is empty, false otherwise
 */
bool instruction_list_empty(struct instruction_list instrs);

/**
 * @brief Add a new instruction to an instruction list
 * @param instrs A pointer to an instruction list
 * @param new_instr A pointer to the new instruction node
 */
void instruction_list_add(struct instruction_list *instrs, struct instruction *new_instr);

/**
 * @brief Merge two instruction list
 * @param instrs1 The first instruction list
 * @param instrs2 The second instruction list
 * @return The merged instruction list
 */
struct instruction_list instruction_list_merge(struct instruction_list instrs1, struct instruction_list instrs2);

