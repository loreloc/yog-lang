
/*! @file ast.h */

#pragma once

#include "token.h"

/*! @brief The types of an abstract syntax tree node */
enum ast_type
{
	AST_NONTERMINAL,
	AST_TERMINAL,
};

/*! @brief The nonterminal types of an abstract syntax tree node */
enum ast_nonterminal_type
{
	AST_NT_SOURCE,
	AST_NT_VARIABLES,
	AST_NT_STATEMENTS,
	AST_NT_ASSIGN,
	AST_NT_INPUT,
	AST_NT_OUTPUT,
	AST_NT_BRANCH,
	AST_NT_EXPRESSION,
	AST_NT_TERM,
	AST_NT_FACTOR,
	AST_NT_CONDITION
};

/*! @brief Abstract Syntax Tree data structure */
struct ast
{
	/*! @brief The type of the node */
	enum ast_type type;

	union
	{
		/*! @brief The nonterminal value of the node */
		enum ast_nonterminal_type nt;

		/*! @brief The terminal value of the node */
		struct token tok;
	};

	/*! @brief The number of children */
	size_t children_cnt;

	/*! @brief The children of the node */
	struct ast **children;
};

/**
 * @brief Make a new abstract syntax tree with a nonterminal node
 * @param nt The nonterminal value of the node
 * @return A new abstract syntax tree
 */
struct ast *ast_make_nonterminal(enum ast_nonterminal_type nt);

/**
 * @brief Make a new abstract syntax tree with a terminal node
 * @param tok The terminal value of the node
 * @return A new abstract syntax tree
 */
struct ast *ast_make_terminal(struct token tok);

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

