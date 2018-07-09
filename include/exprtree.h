
/*! @file exprtree.h */

#pragma once

#include "symtable.h"

/*! @brief The types of an expression node */
enum expr_node_type
{
	EXPR_NODE_OPERATOR,
	EXPR_NODE_LITERAL,
	EXPR_NODE_VARIABLE
};

/*! @brief The supported operators */
enum operator
{
	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV
};

/*! @brief The expression tree data structure */
struct expr_tree
{
	/*! @brief The type of the node */
	enum expr_node_type type;

	union
	{
		/*! @brief The operator value */
		enum operator op;

		/*! @brief The literal value */
		int64_t lit;

		/*! @brief The symbol pointer value */
		struct symbol *sym;

	} value; /*!< @brief The value of the expression tree node */

	/*! @brief The left sub-tree */
	struct expr_tree *left;

	/*! @brief The right sub-tree */
	struct expr_tree *right;
};

/**
 * @brief Allocate a new expression tree with a operator node
 * @param op The operator value
 */
struct expr_tree *expr_tree_make_operator(enum operator op);

/**
 * @brief Allocate a new expression tree with a literal node
 * @param lit The literal value
 */
struct expr_tree *expr_tree_make_literal(int64_t lit);

/**
 * @brief Allocate a new expression tree with a symbol pointer node
 * @param sym The symbol pointer value
 */
struct expr_tree *expr_tree_make_symbol(struct symbol *sym);

/**
 * @brief Clear an expression tree
 * @param tree A pointer to the expression tree to clear
 */
void expr_tree_clear(struct expr_tree *tree);

/**
 * @brief Evaluate an expression tree
 * @param tree A pointer to the expression tree to evaluate
 * @return The value of the expression
 */
int64_t expr_tree_eval(struct expr_tree *tree);

