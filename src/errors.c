
#include "errors.h"

void error_handler_init(struct error_handler *err_hnd)
{
	err_hnd->errs_cnt = 0;
	err_hnd->errs = malloc(ERRORS_MAX_CNT * sizeof(struct error));
}

void error_handler_clear(struct error_handler *err_hnd)
{
	// free the error array
	free(err_hnd->errs);
	err_hnd->errs = NULL;
	err_hnd->errs_cnt = 0;
}

bool error_handler_add(struct error_handler *err_hnd, struct location loc, enum error_type type, const char *msg)
{
	// check for errors overflow
	if(err_hnd->errs_cnt >= ERRORS_MAX_CNT)
		return false;

	// add a new error to the error array
	const size_t new_err_index = err_hnd->errs_cnt++;
	err_hnd->errs[new_err_index].loc = loc;
	err_hnd->errs[new_err_index].type = type;
	strncpy(err_hnd->errs[new_err_index].msg, msg, ERROR_MSG_SIZE);

	return true;
}

