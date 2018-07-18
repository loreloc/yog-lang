
/*! @file semanter.h */

#pragma once

#include "ast.h"
#include "error.h"
#include "instruction.h"

/*! @brief The semantic context data structure */
struct semantic_context
{
	/*! @brief A pointer to the symbol table to modify */
	struct symbol_table *st;

	/*! @brief A pointer to an error list */
	struct error_list *errs;

	/*! @brief A pointer to the abstract syntax tree to analyse */
	struct ast *tree;
};

/**
 * @brief Initialize a semantic context
 * @param ctx A pointer to the semantic context to initialize
 * @param st A pointer to the symbol table to modify
 * @param errs A pointer to the error list
 * @param tree The abstract syntax tree to analyse
 */
void semantic_context_init(struct semantic_context *ctx, struct symbol_table *st, struct error_list *errs, struct ast *tree);

/**
 * @brief Analyse the abstract syntax tree holded by the semantic context
 * @param ctx A pointer to the semantic context
 * @return The instruction list
 */
struct instr_list semantic_context_analyse(struct semantic_context *ctx);

