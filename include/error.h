
/*! @file error.h */

#pragma once

#include "token.h"

/*! @brief The types of an error node */
enum error_type
{
	ERROR_INVALID_TOKEN,
	ERROR_UNEXPECTED_TOKEN,
	ERROR_UNDECLARED_VAR,
	ERROR_MULTIPLE_DECL
};

/*! @brief The node of the error list */
struct error
{
	/*! @brief The location in the source code */
	struct location loc;

	/*! @brief The type of the error */
	enum error_type type;

	union
	{
		struct
		{
			/*! @brief The text string */
			char text[ID_STR_SIZE];

		} lexical;

		struct
		{
			/*! @brief The actual token type found */
			token_type_t actual;

			/*! @brief The expected token types bit set */
			token_type_t expected;

		} syntactic;

		struct
		{
			/*! @brief The location where the variable was declared first */
			struct location first;

			/*! @brief A symbol pointer to the undefined variable */
			struct symbol *sym;

		} semantic;

	} info; /*!< @brief The informations about the error */

	/*! @brief The next error in the error list */
	struct error *next;
};

/*! @brief The error list data structure */
struct error_list
{
	/*! @brief The head pointer of the list */
	struct error *head;

	/*! @brief The tail pointer of the list */
	struct error *tail;
};

/**
 * @brief Make a new invalid token lexical error
 * @param loc The error location in the source code
 * @param text The string message that rappresents the lexical error
 * @return A new error
 */
struct error *error_make_invalid_token(struct location loc, const char *text);

/**
 * @brief Make a new unexpected token syntactic error
 * @param loc The error location in the source code
 * @param actual The actual token type found
 * @param expected The expected token types bit set
 * @return A new error
 */
struct error *error_make_unexpected_token(struct location loc, token_type_t actual, token_type_t expected);

/**
 * @brief Make a new undeclared variable semantic error
 * @param loc The error location in the source code
 * @param sym A pointer to the undeclared variable symbol
 * @return A new error
 */
struct error *error_make_undeclared_var(struct location loc, struct symbol *sym);

/**
 * @brief Make a new multiple declaration semantic error
 * @param loc The error location in the source code
 * @param first The location where the variable was declared first
 * @param sym A pointer to the multiple declared variable symbol
 * @return A new error
 */
struct error *error_make_multiple_decl(struct location loc, struct location first, struct symbol *sym);

/**
 * @brief Initialize an error list
 * @param errs A pointer to the error list to initialize
 */
void error_list_init(struct error_list *errs);

/**
 * @brief Clear an error list
 * @param errs A pointer to the error list to clear
 */
void error_list_clear(struct error_list *errs);

/**
 * @brief Check if an error list is empty
 * @param errs The error list to check
 * @return true if the error list is empty, false otherwise
 */
bool error_list_empty(struct error_list errs);

/**
 * @brief Print an error list
 * @param errs The error list to print
 */
void error_list_show(struct error_list errs);

/**
 * @brief Add a new error node to an error list
 * @param errs A pointer to an error list
 * @param new_err A pointer to the new error node
 */
void error_list_add(struct error_list *errs, struct error *new_err);


