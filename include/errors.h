
/*! @file errors.h */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"
#include "location.h"

/*! The maximum number of errors that an error handler can hold */
#define ERRORS_MAX_CNT 128

/*! The maximum size of the error message string */
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
	struct location loc;      /*!< The location in the source code */
	enum error_type type;     /*!< The type of the error */
	char msg[ERROR_MSG_SIZE]; /*!< An helpful message */
};

/**
 * @brief Error handler
 */
struct error_handler
{
	size_t errors_cnt;    /*!< The number of errors registered */
	struct error *errors; /*!< The errors array */
};

/**
 * @brief Initialize an error handler
 * @param err_hnd A pointer to the error handler to initialize
 */
void error_handler_init(struct error_handler *err_hnd);

/**
 * @brief Clear the resources holded by an error handler
 * @param err_hnd A pointer to the error handler to clean
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
bool error_handler_add(struct error_handler *err_hnd, struct location loc, enum error_type type, const char* msg);


