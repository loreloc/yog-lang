
/*! @file token.h */

#pragma once

#include <stdint.h>

/*! The maximum size of the token string */
#define TOKEN_STR_SIZE 32

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

		char str[TOKEN_STR_SIZE]; /*!< The string field */
	};
};

