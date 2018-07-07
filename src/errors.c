
#include "errors.h"

void print_token_type_set(enum token_type set);

void error_list_init(struct error_list *errs)
{
	errs->head = NULL;
	errs->tail = NULL;
}

void error_list_clear(struct error_list *errs)
{
	// clear the error list
	struct error *tmp;
	while(errs->head != NULL)
	{
		tmp = errs->head;
		errs->head = tmp->next;
		yfree(tmp);
	}
}

void error_list_show(struct error_list errs)
{
	size_t index = 1;
	struct error *tmp = errs.head;
	while(tmp != NULL)
	{
		printf("(%lu) %lu, %lu - ", index, tmp->loc.row, tmp->loc.col);

		switch(tmp->type)
		{
			case ERROR_LEXICAL:
				printf("invalid token \"%s\"\n", 
					tmp->info.lexical.text);
				break;
			case ERROR_SYNTACTIC:
				printf("expected token ");
				print_token_type_set(tmp->info.syntactic.etypes);
				printf(" but found token \"%s\"\n",
					token_type_str(tmp->info.syntactic.atype));
				break;
		}

		index++;
		tmp = tmp->next;
	}
}

void error_list_add_lexical(struct error_list *errs, struct location loc, const char *text)
{
	// allocate a new error node
	struct error *new_error = ymalloc(sizeof(struct error));
	new_error->next = NULL;
	new_error->loc = loc;
	new_error->type = ERROR_LEXICAL;
	snprintf(new_error->info.lexical.text, ID_STR_SIZE, "%s", text);

	// add the new error node to the error list
	if(errs->head)
	{
		errs->tail->next = new_error;
		errs->tail = new_error;
	}
	else
	{
		errs->head = new_error;
		errs->tail = new_error;
	}
}

void error_list_add_syntactic(struct error_list *errs, struct location loc, enum token_type atype, enum token_type etypes)
{
	// allocate a new error node
	struct error *new_error = ymalloc(sizeof(struct error));
	new_error->next = NULL;
	new_error->loc = loc;
	new_error->type = ERROR_SYNTACTIC;
	new_error->info.syntactic.atype = atype;
	new_error->info.syntactic.etypes = etypes;

	// add the new error node to the error list
	if(errs->head)
	{
		errs->tail->next = new_error;
		errs->tail = new_error;
	}
	else
	{
		errs->head = new_error;
		errs->tail = new_error;
	}
}

void print_token_type_set(enum token_type set)
{
	int mask = 1;
	int val = 0;

	while(val == 0)
	{
		val = (int)set & mask;
		mask <<= 1;
	}

	printf("\"%s\"", token_type_str((enum token_type)val));

	while(mask <= TOKEN_TYPE_MAX)
	{
		val = (int)set & mask;

		if(val)
			printf(" or \"%s\"", token_type_str((enum token_type)val));
	
		mask <<= 1;
	}
}

