
/*! @file scanner.h */

#pragma once

#include "error.h"
#include "token.h"

/*! @brief The lexical context data structure */
struct lex_context
{
	/*! @brief The source code file */
	FILE* source;

	/*! @brief The lookahead character */
	char lookahead;

	/*! @brief The current location in the source code */
	struct location loc;
};

/**
 * @brief Initialize a lexical context
 * @param ctx A pointer to the lexical context to initialize
 * @param source The source code file
 */
void lex_context_init(struct lex_context *ctx, FILE *source);

/**
 * @brief Get the next token from the token stream
 * @param ctx A pointer to the lexical context
 * @param st A pointer to the symbol table
 * @param errs A pointer to the error list
 * @return The next token value
 */
struct token lex(struct lex_context *ctx, struct symbol_table *st, struct error_list *errs);

