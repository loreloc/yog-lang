
/*! @file scanner.h */

#pragma once

#include "errors.h"
#include "token.h"

/**
 * @brief The lexical context
 */
struct lex_context
{
	FILE* source;        /*!< @brief The source code file */
	char lookahead;      /*!< @brief The lookahead character */
	struct location loc; /*!< @brief The current location in the source code */
};

/**
 * @brief Initialize a lexical context
 * @param ctx A pointer to the lexical context to initialize
 * @param source The source file pointer
 */
void lex_context_init(struct lex_context *ctx, FILE *source);

/**
 * @brief Get the next token
 * @param ctx A pointer to the lexical context
 * @param st A pointer to the symbol table
 * @param errs A pointer to the error list
 * @return The next token value
 */
struct token lex(struct lex_context *ctx, struct symbol_table *st, struct error_list *errs);

