
#include "error.h"

void print_expected_tokens(enum token_type types);

struct error *error_make_lexical(struct location loc, const char *text)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_LEXICAL;
	snprintf(err->lex_info.text, ID_STR_SIZE, "%s", text);

	return err;
}

struct error *error_make_syntactic(struct location loc, enum token_type actual, enum token_type expected)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_SYNTACTIC;
	err->syn_info.actual = actual;
	err->syn_info.expected = expected;

	return err;
}

struct error *error_make_semantic(struct location loc, struct symbol *sym)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_SEMANTIC;
	err->sem_info.sym = sym;

	return err;
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
				printf("invalid token \"%s\"\n", tmp->lex_info.text);
				break;

			case ERROR_SYNTACTIC:
				printf("expected token ");
				print_expected_tokens(tmp->syn_info.expected);
				printf("but found token \"%s\"\n", token_type_str(tmp->syn_info.actual));
				break;

			case ERROR_SEMANTIC:
				printf("undeclared variable \"%s\"\n", tmp->sem_info.sym->id);
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

	while(mask <= TOKEN_TYPE_MAX)
	{
		enum token_type type = types & mask;

		if(type != 0)
			printf("\"%s\" ", token_type_str(type));
	
		mask <<= 1;
	}
}

