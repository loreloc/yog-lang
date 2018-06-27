
#include "errors.h"

void error_handler_init(struct error_handler *err_hnd)
{
	err_hnd->head = err_hnd->tail = NULL;
}

void error_handler_clear(struct error_handler *err_hnd)
{
	// clear the error list
	struct error *tmp;
	while(err_hnd->head != NULL)
	{
		tmp = err_hnd->head;
		err_hnd->head = tmp->next;
		free(tmp);
	}
}

void error_handler_add(struct error_handler *err_hnd, struct location loc, enum error_type type, const char *msg)
{
	// allocate a new error node
	struct error *new_error = malloc(sizeof(struct error));
	new_error->next = NULL;
	new_error->loc = loc;
	new_error->type = type;
	strcpy(new_error->msg, msg);

	// add the new error node to the error list
	if(err_hnd->head)
	{
		err_hnd->tail->next = new_error;
		err_hnd->tail = new_error;
	}
	else
	{
		err_hnd->head = err_hnd->tail = new_error;
	}
}

