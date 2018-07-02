
/*! @file errors.h */

#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "location.h"

/*! @brief The maximum size of the error message string */
#define ERROR_MSG_SIZE ID_STR_SIZE

/**
 * @brief All the possible error types
 */
enum error_type
{
	ERROR_LEXICAL
};

/**
 * @brief Error informations
 */
struct error
{
	struct location loc;      /*!< @brief The location in the source code */
	enum error_type type;     /*!< @brief The type of the error */
	char msg[ERROR_MSG_SIZE]; /*!< @brief An helpful message */
	struct error *next;       /*!< @brief The next error in the error list */
};

/**
 * @brief Error handler
 */
struct error_handler
{
	struct error *head; /*!< @brief The error list head */
	struct error *tail; /*!< @brief The error list tail */
};

/**
 * @brief Initialize an error handler
 * @param err_hnd A pointer to the error handler to initialize
 */
void error_handler_init(struct error_handler *err_hnd);

/**
 * @brief Clear the resources holded by an error handler
 * @param err_hnd A pointer to the error handler to clear
 */
void error_handler_clear(struct error_handler *err_hnd);

/**
 * @brief Notify a new error to an error handler
 * @param err_hnd A pointer to the error handler
 * @param loc The error location in the source code
 * @param type The type of the error
 * @param msg The helpful message string
 * @return false if an error occured, true otherwise
 */
void error_handler_add(struct error_handler *err_hnd, struct location loc, enum error_type type, const char* msg);


