
#include "error.h"

void print_expected_tokens(token_type_t types);

struct error *error_make_invalid_token(struct location loc, const char *text)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_INVALID_TOKEN;
	snprintf(err->info.lexical.text, ID_STR_SIZE, "%s", text);

	return err;
}

struct error *error_make_unexpected_token(struct location loc, token_type_t actual, token_type_t expected)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_UNEXPECTED_TOKEN;
	err->info.syntactic.actual = actual;
	err->info.syntactic.expected = expected;

	return err;
}

struct error *error_make_undeclared_var(struct location loc, struct symbol *sym)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_UNDECLARED_VAR;
	err->info.semantic.sym = sym;

	return err;
}

struct error *error_make_multiple_decl(struct location loc, struct location first, struct symbol *sym)
{
	struct error *err = ymalloc(sizeof(struct error));

	err->next = NULL;
	err->loc = loc;
	err->type = ERROR_MULTIPLE_DECL;
	err->info.semantic.first = first;
	err->info.semantic.sym = sym;

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
		printf("(%zu) %zu, %zu - ", index, tmp->loc.row, tmp->loc.col);

		switch(tmp->type)
		{
			case ERROR_INVALID_TOKEN:
				printf("invalid token \"%s\"\n", tmp->info.lexical.text);
				break;

			case ERROR_UNEXPECTED_TOKEN:
				printf("expected token ");
				print_expected_tokens(tmp->info.syntactic.expected);
				printf("but found token \"%s\"\n", token_type_str(tmp->info.syntactic.actual));
				break;

			case ERROR_UNDECLARED_VAR:
				printf("undeclared variable \"%s\"\n", tmp->info.semantic.sym->id);
				break;

			case ERROR_MULTIPLE_DECL:
				printf("multiple declaration of \"%s\", first defined at %lu, %lu\n",
					tmp->info.semantic.sym->id, tmp->info.semantic.first.row, tmp->info.semantic.first.col);
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

void print_expected_tokens(token_type_t types)
{
	token_type_t mask = 1;

	while(mask <= TOKEN_TYPE_MAX)
	{
		token_type_t type = types & mask;

		if(type != 0)
			printf("\"%s\" ", token_type_str(type));
	
		mask <<= 1;
	}
}

