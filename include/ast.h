
/*! @file ast.h */

#pragma once

#include <stdarg.h>
#include "symtable.h"

/*! @brief The types of an abstract syntax tree node */
enum ast_node_type
{
	AST_NODE_SOURCE,
	AST_NODE_VARIABLES,
	AST_NODE_STATEMENTS,
	AST_NODE_ASSIGN,
	AST_NODE_INPUT,
	AST_NODE_OUTPUT,
	AST_NODE_EXPRESSION,
	AST_NODE_TERM,
	AST_NODE_FACTOR,
	AST_NODE_VAR,
	AST_NODE_BEGIN,
	AST_NODE_END,
	AST_NODE_INT,
	AST_NODE_COLON,
	AST_NODE_SEMICOLON,
	AST_NODE_READ,
	AST_NODE_WRITE,
	AST_NODE_LITERAL,
	AST_NODE_IDENTIFIER,
	AST_NODE_PLUS,
	AST_NODE_MINUS,
	AST_NODE_MUL,
	AST_NODE_DIV,
	AST_NODE_EQUAL
};

/*! @brief Abstract Syntax Tree data structure */
struct ast
{
	/*! @brief The type of the node */
	enum ast_node_type type;

	union
	{
		/*! @brief The literal value of the node */
		int64_t lit;

		/*! @brief A pointer to a symbol value of the node */
		struct symbol *sym;

	} value; /*!< @brief The value of the node */

	/*! @brief The next abstract syntax tree at the same level */
	struct ast *next;

	/*! @brief The next abstract syntax tree at an inferior level*/
	struct ast *subtree;
};

/**
 * @brief Make a new abstract syntax tree
 * @param type The type of the new node
 * @return A new abstract syntax tree
 */
struct ast *ast_make(enum ast_node_type type);

/**
 * @brief Clear an abstract syntax tree
 * @param tree A pointer to the abstract syntax tree to clear
 */
void ast_clear(struct ast *tree);

/**
 * @brief Add a new abstract syntax sub-tree to a node
 * @param tree A pointer to the node to modify
 * @param new_tree The new abstract syntax tree to add
 */
void ast_add_subtree(struct ast *tree, struct ast *new_tree);

/**
 * @brief Add a new abstract syntax sub-tree to a node
 * @param tree A pointer to the node to modify
 * @param type The type of the node of the abstract syntax tree to add
 * @return A pointer to the added sub-tree
 */
struct ast *ast_add_subtree_node(struct ast *tree, enum ast_node_type type);

/**
 * @brief Add a new abstract syntax sub-tree to a node with a literal node
 * @param tree A pointer to the node to modify
 * @param lit The literal value
 * @return A pointer to the added sub-tree
 */
struct ast *ast_add_subtree_literal(struct ast *tree, int64_t lit);

/**
 * @brief Add a new abstract syntax sub-tree to a node with a symbol pointer node
 * @param tree A pointer to the node to modify
 * @param sym A pointer to a symbol
 * @return A pointer to the added sub-tree
 */
struct ast *ast_add_subtree_symbol(struct ast *tree, struct symbol *sym);

