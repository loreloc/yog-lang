
#include "errors.h"

bool err_handler_init(err_handler_t *err_handler)
{
	err_handler->errors_cnt = 0;
	err_handler->errors = malloc(ERRORS_MAX_CNT * sizeof(err_t));

	if(!err_handler->errors)
		return false;

	return true;
}

void err_handler_clear(err_handler_t *err_handler)
{
	// free the error string messages
	for(size_t i = 0; i < err_handler->errors_cnt; ++i)
		free(err_handler->errors[i].message);

	// free the error array
	free(err_handler->errors);
	err_handler->errors = NULL;
	err_handler->errors_cnt = 0;
}

bool err_handler_add(err_handler_t *err_handler, err_t err)
{
	// check for errors overflow
	if(err_handler->errors_cnt >= ERRORS_MAX_CNT)
		return false;

	// add a new error to the error array
	err_handler->errors[err_handler->errors_cnt++] = err;

	return true;
}

