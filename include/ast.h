
/*! @file ast.h */

#pragma once

#include "symtable.h"

/*! @brief The types of an abstract syntax tree node */
enum ast_type
{
	AST_SOURCE,
	AST_VARIABLES,
	AST_STATEMENTS,
	AST_ASSIGN,
	AST_INPUT,
	AST_OUTPUT,
	AST_EXPRESSION,
	AST_TERM,
	AST_FACTOR,
	AST_VAR,
	AST_BEGIN,
	AST_END,
	AST_INT,
	AST_COLON,
	AST_SEMICOLON,
	AST_READ,
	AST_WRITE,
	AST_LITERAL,
	AST_IDENTIFIER,
	AST_PLUS,
	AST_MINUS,
	AST_MUL,
	AST_DIV,
	AST_EQUAL
};

/*! @brief Abstract Syntax Tree data structure */
struct ast
{
	/*! @brief The type of the node */
	enum ast_type type;

	union
	{
		/*! @brief The literal value of the node */
		int64_t lit;

		/*! @brief A pointer to a symbol value of the node */
		struct symbol *sym;

	} value; /*!< @brief The value of the node */

	/*! @brief The number of children */
	size_t children_cnt;

	/*! @brief The children of the node */
	struct ast **children;
};

/**
 * @brief Make a new abstract syntax tree
 * @param type The type of the new node
 * @return A new abstract syntax tree
 */
struct ast *ast_make(enum ast_type type);

/**
 * @brief Make a new children of literal type
 * @param lit The literal value of the child tree to add
 * @return A new abstract syntax tree
 */
struct ast *ast_make_literal(int64_t lit);

/**
 * @brief Add a new children of symbol pointer type
 * @param sym The symbol pointer value of the child tree to add
 * @return A new abstract syntax tree
 */
struct ast *ast_make_symbol(struct symbol *sym);

/**
 * @brief Add a new children to an abstract syntax tree node
 * @param tree The abstract syntax tree node
 * @param child The new child tree to add
 * @return A pointer to the added child tree
 */
struct ast *ast_add_child(struct ast *tree, struct ast *child);

/**
 * @brief Clear an abstract syntax tree
 * @param tree A pointer to the abstract syntax tree to clear
 */
void ast_clear(struct ast *tree);

