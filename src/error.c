
#include "error.h"

void print_expected_tokens(enum token_type types);

struct error *error_make_lexical(struct location loc, const char *text)
{
	struct error *new_err = ymalloc(sizeof(struct error));

	new_err->next = NULL;
	new_err->loc = loc;
	new_err->type = ERROR_LEXICAL;
	snprintf(new_err->info.lexical.text, ID_STR_SIZE, "%s", text);

	return new_err;
}

struct error *error_make_syntactic(struct location loc, enum token_type actual, enum token_type expected)
{
	struct error *new_err = ymalloc(sizeof(struct error));

	new_err->next = NULL;
	new_err->loc = loc;
	new_err->type = ERROR_SYNTACTIC;
	new_err->info.syntactic.actual = actual;
	new_err->info.syntactic.expected = expected;

	return new_err;
}

struct error *error_make_semantic(struct location loc, struct symbol *sym)
{
	struct error *new_err = ymalloc(sizeof(struct error));

	new_err->next = NULL;
	new_err->loc = loc;
	new_err->type = ERROR_SEMANTIC;
	new_err->info.semantic.sym = sym;

	return new_err;
}

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
				printf("invalid token \"%s\"\n", tmp->info.lexical.text);
				break;

			case ERROR_SYNTACTIC:
				printf("expected token ");
				print_expected_tokens(tmp->info.syntactic.expected);
				printf(" but found token \"%s\"\n", token_type_str(tmp->info.syntactic.actual));
				break;

			case ERROR_SEMANTIC:
				printf("undeclared variable \"%s\"\n", tmp->info.semantic.sym->id);
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

void print_expected_tokens(enum token_type types)
{
	int mask = 1;
	int val = 0;

	while(val == 0 && mask <= TOKEN_TYPE_MAX)
	{
		val = (int)types & mask;
		mask <<= 1;
	}

	printf("\"%s\"", token_type_str((enum token_type)val));

	while(mask <= TOKEN_TYPE_MAX)
	{
		val = (int)types & mask;

		if(val)
			printf(" or \"%s\"", token_type_str((enum token_type)val));
	
		mask <<= 1;
	}
}

