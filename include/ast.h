
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

	/*! @brief The number of sub-trees */
	size_t subtrees_cnt;

	/*! @brief The abstract syntax sub-trees */
	struct ast **subtrees;
};

/**
 * @brief Make a new abstract syntax tree with a non-terminal node
 * @param type The type of the new node
 * @param count The number of abstract syntax subtrees
 * @param ... The abstract syntax subtrees
 * @return A new abstract syntax tree
 */
struct ast *ast_make_nonterminal(enum ast_node_type type, size_t count, ...);

/**
 * @brief Make a new abstract syntax tree with a terminal node
 * @param type The type of the new node
 * @return A new abstract syntax tree
 */
struct ast *ast_make_terminal(enum ast_node_type type);

/**
 * @brief Clear an abstract syntax tree
 * @param tree A pointer to the abstract syntax tree to clear
 */
void ast_clear(struct ast *tree);

