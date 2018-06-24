
/*! @file scanner.h */

#pragma once

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "errors.h"
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
 * @param result A pointer to the scan result
 * @param scanner A pointer to the lexical analyser
 * @param err_handler A pointer to the error handler
 * @return false if an error occured, true otherwise
 */
bool scan(scan_result_t *result, scanner_t *scanner, err_handler_t *err_handler);

