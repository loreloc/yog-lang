
/*! @file errors.h */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <location.h>

/*! The maximum number of errors that an error handler can hold */
#define ERRORS_MAX_CNT 128

/**
 * @brief All the possible error types
 */
typedef enum
{
	ERROR_LEXICAL

} err_type_t;

/**
 * @brief Error informations
 */
typedef struct
{
	location_t location;
	err_type_t type;
	char *message;

} err_t;

/**
 * @brief Error handler
 */
typedef struct
{
	size_t errors_cnt;
	err_t *errors;

} err_handler_t;

/**
 * @brief Initialize an error handler
 * @param err_handler A pointer to the error handler to initialize
 * @return false if an error occured, true otherwise
 */
bool err_handler_init(err_handler_t *err_handler);

/**
 * @brief Clear the resources holded by an error handler
 * @param err_handler A pointer to the error handler to clean
 */
void err_handler_clear(err_handler_t *err_handler);

/**
 * @brief Notify a new error to an error handler
 * @param err_handler A pointer to the error handler
 * @param err The error to add to the error array
 * @return false if an error occured, true otherwise
 */
bool err_handler_add(err_handler_t *err_handler, err_t err);


