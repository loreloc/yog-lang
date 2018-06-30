
/*! @file scanner.h */

#pragma once

#include <stdio.h>
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
 * @param tok A pointer to the token rsult
 * @param ctx A pointer to the lexical context
 * @param st A pointer to the symbol table
 * @param err_hnd A pointer to the error handler
 * @return false if source EOF has been reached, true otherwise
 */
bool lex(struct token *tok, struct lex_context *ctx, struct symbol_table *st, struct error_handler *err_hnd);

