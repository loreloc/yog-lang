
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

bool error_list_empty(struct error_list errs)
{
	return errs.head == NULL;
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

void error_list_add(struct error_list *errs, struct error *new_err)
{
	if(error_list_empty(*errs))
	{
		errs->head = new_err;
		errs->tail = new_err;
	}
	else
	{
		errs->tail->next = new_err;
		errs->tail = new_err;
	}
}

void error_list_add_lexical(struct error_list *errs, struct location loc, const char *text)
{
	// allocate a new error node
	struct error *new_err = ymalloc(sizeof(struct error));
	new_err->next = NULL;
	new_err->loc = loc;
	new_err->type = ERROR_LEXICAL;
	snprintf(new_err->info.lexical.text, ID_STR_SIZE, "%s", text);

	error_list_add(errs, new_err);
}

void error_list_add_syntactic(struct error_list *errs, struct location loc, enum token_type atype, enum token_type etypes)
{
	// allocate a new error node
	struct error *new_err = ymalloc(sizeof(struct error));
	new_err->next = NULL;
	new_err->loc = loc;
	new_err->type = ERROR_SYNTACTIC;
	new_err->info.syntactic.atype = atype;
	new_err->info.syntactic.etypes = etypes;

	error_list_add(errs, new_err);
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

