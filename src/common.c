
#include "common.h"

void *ymalloc(size_t size)
{
	if(size == 0)
		size = 1;

	void *ptr = malloc(size);
	if(ptr == NULL)
	{
		fputs("memory allocation failed\n", stderr);
		exit(~0x0);
	}

	return ptr;
}

void *ycalloc(size_t num, size_t size)
{
	if(size == 0)
		size = 1;
	
	void *ptr = calloc(num, size);
	if(ptr == NULL)
	{
		fputs("memory allocation failed\n", stderr);
		exit(~0x0);
	}

	return ptr;
}

void *yrealloc(void *ptr, size_t size)
{
	if(size == 0)
		size = 1;

	ptr = realloc(ptr, size);
	if(ptr == NULL)
	{
		fputs("memory reallocation failed\n", stderr);
		exit(~0x0);
	}

	return ptr;
}

void yfree(void *ptr)
{
	if(ptr != NULL)
		free(ptr);
}

char *ystrdup(char *str)
{
	char *ptr = ymalloc(strlen(str) + 1);

	strcpy(ptr, str);

	return ptr;
}

