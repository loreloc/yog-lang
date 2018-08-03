
/*! @file token.h */

#pragma once

#include "symtable.h"

#define TOKEN_EOF        (1ull <<  0)
#define TOKEN_VAR        (1ull <<  1)
#define TOKEN_BEGIN      (1ull <<  2)
#define TOKEN_END        (1ull <<  3)
#define TOKEN_INT        (1ull <<  4)
#define TOKEN_READ       (1ull <<  5)
#define TOKEN_WRITE      (1ull <<  6)
#define TOKEN_COLON      (1ull <<  7)
#define TOKEN_SEMICOLON  (1ull <<  8)
#define TOKEN_LITERAL    (1ull <<  9)
#define TOKEN_IDENTIFIER (1ull << 10)
#define TOKEN_PLUS       (1ull << 11)
#define TOKEN_MINUS      (1ull << 12)
#define TOKEN_MUL        (1ull << 13)
#define TOKEN_DIV        (1ull << 14)
#define TOKEN_ASSIGN     (1ull << 15)
#define TOKEN_LPAREN     (1ull << 16)
#define TOKEN_RPAREN     (1ull << 17)
#define TOKEN_TYPE_MAX   TOKEN_RPAREN

/*! @brief The token type */
typedef uint64_t token_type_t;

/*! @brief The token data structure */
struct token
{
	/*! @brief The location of the token in the source code */
	struct location loc;
	
	/*! @brief The type of the token */
	token_type_t type;

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
const char *token_type_str(token_type_t type);

