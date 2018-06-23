
/*! @file token.h */

#pragma once

#include <stdint.h>

/**
 * @brief The types of a token
 */
typedef enum
{
	TOKEN_VAR,
	TOKEN_BEGIN,
	TOKEN_END,
	TOKEN_INT,
	TOKEN_COLON,
	TOKEN_SEMICOLON,
	TOKEN_LITERAL,
	TOKEN_IDENTIFIER,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_EQUAL

} token_type_t;

/**
 * @brief The token data structure
 */
typedef struct
{
	token_type_t type;

	union
	{
		int64_t integer;
		char* string;
	};

} token_t;

