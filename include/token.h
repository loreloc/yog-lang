
/*! @file token.h */

#pragma once

#include <stdint.h>
#include "config.h"

/**
 * @brief The possible types of a token
 */
enum token_type
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
};

/**
 * @brief The token data structure
 */
struct token
{
	enum token_type type; /*!< The type of the token */

	union
	{
		int64_t lit; /*!< The literal field */

		char id[ID_STR_SIZE]; /*!< The identifier field */
	};
};

