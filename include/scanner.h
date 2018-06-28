
/*! @file scanner.h */

#pragma once

#include <stdio.h>
#include "symtable.h"
#include "errors.h"
#include "token.h"

/**
 * @brief The result of a lexical analysis step
 */
struct lex_result
{
	struct token tok;    /*!< @brief The token value */
	struct location loc; /*!< @brief The location of the token in the source code */
};

/**
 * @brief The lexical analyser
 */
struct scanner
{
	FILE* source;        /*!< @brief The source code file */
	char lookahead;      /*!< @brief The lookahead character */
	struct location loc; /*!< @brief The current location in the source code */
};

/**
 * @brief Initialize a lexical analyser
 * @param scn A pointer to the lexical analyser to initialize
 * @param source The source file pointer
 */
void scanner_init(struct scanner *scn, FILE *source);

/**
 * @brief Execute a lexical analysis step
 * @param res A pointer to the result
 * @param scn A pointer to the lexical analyser
 * @param st A pointer to the symbol table
 * @param err_hnd A pointer to the error handler
 * @return false if an error occured, true otherwise
 */
bool scanner_lex(struct lex_result *res, struct scanner *scn, struct symbol_table *st, struct error_handler *err_hnd);

