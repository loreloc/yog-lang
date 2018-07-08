
/*! @file exprtree.h */

#pragma once

#include "symtable.h"

/**
 * @brief The possible expression tree node types
 */
enum expr_node_type
{
	NODE_OPERATOR,
	NODE_LITERAL,
	NODE_VARIABLE
};

/**
 * @brief The supported operators
 */
enum operator
{
	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV
};

/**
 * @brief The expression tree data structure
 */
struct expr_tree
{
	enum expr_node_type type; /*!< @brief The type of the node */

	union
	{
		enum operator op;   /*!< @brief The operator field */
		int64_t lit;        /*!< @brief The literal field */
		struct symbol *sym; /*!< @brief The symbol pointer field */

	} value; /*!< @brief The value of the expression tree node */

	struct expr_tree *left;  /*!< @brief The left sub-tree */
	struct expr_tree *right; /*!< @brief The right sub-tree */
};

/**
 * @brief Allocate a new expression tree with operator node
 * @param op The operator value
 */
struct expr_tree *expr_tree_make_op(enum operator op);

/**
 * @brief Allocate a new expression tree with literal node
 * @param lit The literal value
 */
struct expr_tree *expr_tree_make_lit(int64_t lit);

/**
 * @brief Allocate a new expression tree with symbol pointer node
 * @param sym The symbol pointer
 */
struct expr_tree *expr_tree_make_sym(struct symbol *sym);

/**
 * @brief Clear an expression tree
 * @param tree A pointer to the expression tree to clear
 */
void expr_tree_clear(struct expr_tree *tree);

/**
 * @brief Evalueate an expression tree
 * @param tree A pointer to the expression tree to evaluate
 * @return The value of the expression
 */
int64_t expr_tree_eval(struct expr_tree *tree);

