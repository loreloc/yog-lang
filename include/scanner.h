
/*! @file scanner.h */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "location.h"
#include "token.h"

/**
 * @brief The result of the scan function
 */
typedef struct
{
	token_t token;
	location_t location;

} scan_result_t;

/**
 * @brief The lexical analyser
 */
typedef struct
{
	FILE* file;
	char lookahead;
	location_t cursor;

} scanner_t;

/**
 * @brief Initialize a lexical analyser
 * @param scanner A pointer to the lexical analyser to initialize
 * @param file The source file pointer
 */
void scanner_init(scanner_t *scanner, FILE *file);

/**
 * @brief Execute a lexical analysis step
 * @param scanner A pointer to the lexical analyser
 */
scan_result_t scan(scanner_t *scanner);

