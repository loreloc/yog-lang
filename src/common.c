
#include "common.h"

char *string_duplicate(const char *str)
{
	char *new_str = malloc(strlen(str)+1);
	if(!new_str)
		return NULL;

	strcpy(new_str, str);

	return new_str;
}

