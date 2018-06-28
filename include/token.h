
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
	TOKEN_READ,
	TOKEN_WRITE,
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
	enum token_type type; /*!< @brief The type of the token */

	union
	{
		int64_t lit; /*!< @brief The literal field */
		id_ptr_t id; /*!< @brief The identifier field */
	};
};

