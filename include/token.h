
/*! @file token.h */

#pragma once

#include "location.h"
#include "symtable.h"

/*! @brief The types of a token */
enum token_type
{
	TOKEN_EOF        = (1 <<  0),
	TOKEN_VAR        = (1 <<  1),
	TOKEN_BEGIN      = (1 <<  2),
	TOKEN_END        = (1 <<  3),
	TOKEN_INT        = (1 <<  4),
	TOKEN_READ       = (1 <<  5),
	TOKEN_WRITE      = (1 <<  6),
	TOKEN_COLON      = (1 <<  7),
	TOKEN_SEMICOLON  = (1 <<  8),
	TOKEN_LITERAL    = (1 <<  9),
	TOKEN_IDENTIFIER = (1 << 10),
	TOKEN_PLUS       = (1 << 11),
	TOKEN_MINUS      = (1 << 12),
	TOKEN_MUL        = (1 << 13),
	TOKEN_DIV        = (1 << 14),
	TOKEN_EQUAL      = (1 << 15),
	TOKEN_LPAREN     = (1 << 16),
	TOKEN_RPAREN     = (1 << 17)
};

/*! @brief The maximum token type value */
#define TOKEN_TYPE_MAX (int)(TOKEN_RPAREN)

/*! @brief The token data structure */
struct token
{
	/*! @brief The location of the token in the source code */
	struct location loc;
	
	/*! @brief The type of the token */
	enum token_type type;

	union
	{
		/*! @brief The literal value */
		int64_t lit;

		/*! @brief A pointer to a symbol value */
		struct symbol *sym;
	};
};

/**
 * @brief Translate a token type to a string
 * @param type The token type to translate
 * @return A constant string that rappresents the token type
 */
const char *token_type_str(enum token_type type);

