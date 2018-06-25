
/*! @file scanner.h */

#pragma once

#include <stdio.h>
#include "errors.h"
#include "token.h"

/**
 * @brief The result of the scan function
 */
struct scan_result
{
	struct token tok;    /*!< @brief The token value of the scan result */
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
 * @param res A pointer to the scan result
 * @param scn A pointer to the lexical analyser
 * @param err_hnd A pointer to the error handler
 * @return false if an error occured, true otherwise
 */
bool scan(struct scan_result *res, struct scanner *scn, struct error_handler *err_hnd);

