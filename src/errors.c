
#include "errors.h"

void print_token_type_set(enum token_type set);

void error_list_init(struct error_list *errs)
{
	errs->head = NULL;
	errs->tail = NULL;
}

void error_list_clear(struct error_list *errs)
{
	struct error *tmp;
	while(errs->head != NULL)
	{
		tmp = errs->head;
		errs->head = errs->head->next;
		yfree(tmp);
	}

	errs->tail = NULL;
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
				printf("invalid token \"%s\"\n",  tmp->info.lexical.text);
				break;
			case ERROR_SYNTACTIC:
				printf("expected token ");
				print_token_type_set(tmp->info.syntactic.exp_types);
				printf(" but found token \"%s\"\n", token_type_str(tmp->info.syntactic.act_type));
				break;
		}

		index++;
		tmp = tmp->next;
	}
}

void error_list_add(struct error_list *errs, struct error *new_node)
{
	if(error_list_empty(*errs))
	{
		errs->head = new_node;
		errs->tail = new_node;
	}
	else
	{
		errs->tail->next = new_node;
		errs->tail = new_node;
	}
}

void error_list_add_lexical(struct error_list *errs, struct location loc, const char *text)
{
	struct error *new_node = ymalloc(sizeof(struct error));

	new_node->next = NULL;
	new_node->loc = loc;
	new_node->type = ERROR_LEXICAL;
	snprintf(new_node->info.lexical.text, ID_STR_SIZE, "%s", text);

	error_list_add(errs, new_node);
}

void error_list_add_syntactic(struct error_list *errs, struct location loc, enum token_type act_type, enum token_type exp_types)
{
	struct error *new_node = ymalloc(sizeof(struct error));

	new_node->next = NULL;
	new_node->loc = loc;
	new_node->type = ERROR_SYNTACTIC;
	new_node->info.syntactic.act_type = act_type;
	new_node->info.syntactic.exp_types = exp_types;

	error_list_add(errs, new_node);
}

void print_token_type_set(enum token_type set)
{
	int mask = 1;
	int val = 0;

	while(val == 0 && mask <= TOKEN_TYPE_MAX)
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

