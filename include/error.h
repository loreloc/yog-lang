
/*! @file error.h */

#pragma once

#include "token.h"

/*! @brief The types of an error node */
enum error_type
{
	ERROR_LEXICAL,
	ERROR_SYNTACTIC,
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
			enum token_type act_type;

			/*! @brief The expected token types bit set */
			enum token_type exp_types;

		} syntactic;

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
 * @param new_node A pointer to the new error node
 */
void error_list_add(struct error_list *errs, struct error *new_node);

/**
 * @brief Add a new lexical error to an error list
 * @param errs A pointer to an error list
 * @param loc The error location in the source code
 * @param text The string message that rappresents the lexical error
 */
void error_list_add_lexical(struct error_list *errs, struct location loc, const char *text);

/**
 * @brief Add a new syntactic error to an error list
 * @param errs A pointer to an error list
 * @param loc The error location in the source code
 * @param act_type The actual token type found
 * @param exp_types The expected token types bit set
 */
void error_list_add_syntactic(struct error_list *errs, struct location loc, enum token_type act_type, enum token_type exp_types);


