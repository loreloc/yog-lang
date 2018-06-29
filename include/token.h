
/*! @file token.h */

#pragma once

#include "location.h"
#include "symtable.h"

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
	struct location loc;  /*!< @brief The location of the token in the source code */
	enum token_type type; /*!< @brief The type of the token */

	union
	{
		int64_t lit;        /*!< @brief The literal field */
		struct symbol *sym; /*!< @brief The symbol pointer field */
	};
};

