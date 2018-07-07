
/*! @file errors.h */

#pragma once

#include <stdarg.h>
#include "token.h"

/*! @brief The size of the expected tokens and found token strings */
#define TOKEN_STR_SIZE 64

/**
 * @brief All the possible error types
 */
enum error_type
{
	ERROR_LEXICAL,
	ERROR_SYNTACTIC,
};

/**
 * @brief The node of an error list
 */
struct error
{
	struct location loc;  /*!< @brief The location in the source code */
	enum error_type type; /*!< @brief The type of the error */

	union
	{
		struct
		{
			char text[ID_STR_SIZE]; /*!< @brief The text string */

		} lexical;

		struct
		{
			enum token_type atype;  /*!< @brief The actual token type */
			enum token_type etypes; /*!< @brief The expected token types set */

		} syntactic;

	} info; /*!< @brief The informations about the error */

	struct error *next; /*!< @brief The next error in the error list */
};

/**
 * @brief The error list
 */
struct error_list
{
	struct error *head; /*!< @brief The error list head */
	struct error *tail; /*!< @brief The error list tail */
};

/**
 * @brief Initialize an error handler
 * @param errs A pointer to the error list to initialize
 */
void error_list_init(struct error_list *errs);

/**
 * @brief Clear the resources holded by an error handler
 * @param errs A pointer to the error list to clear
 */
void error_list_clear(struct error_list *errs);

/**
 * @brief Print an error list
 * @param errs The error list to print
 */
void error_list_show(struct error_list errs);

/**
 * @brief Add a new lexical error to an error list
 * @param errs A pointer to an error list
 * @param loc The error location in the source code
 * @param text The string message that rappresents the lexical error
 */
void error_list_add_lexical(struct error_list *errs, struct location loc, const char *text);

/**
 * @brief Add a new syntattic error to an error list
 * @param errs A pointer to an error list
 * @param loc The error location in the source code
 * @param atype The actual token type
 * @param etypes The expected token types set
 */
void error_list_add_syntactic(struct error_list *errs, struct location loc, enum token_type atype, enum token_type etypes);


