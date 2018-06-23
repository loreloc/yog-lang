
/*! @file scanner.h */

#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "location.h"
#include "token.h"

/**
 * @brief The lexical analyser
 */
typedef struct
{
	FILE* source_file;
	location_t cursor;
	char next_char;
	bool lookahead;

} scanner_t;

/**
 * @brief The result of a lexical analysis step
 */
typedef struct
{
	bool valid;
	token_t token;
	location_t location;

} lex_result_t;

/**
 * @brief Initialize the lexical analyser
 * @param scanner A pointer to the scanner to initialize
 * @param source_file The source code file pointer
 */
void scanner_init(scanner_t *scanner, FILE *source_file);

