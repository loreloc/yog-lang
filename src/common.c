
#include <assert.h>
#include "common.h"

void yassert_fail(bool val, const char *msg, const char *file, size_t line)
{
	if(val == false)
	{
		fprintf(stderr, "assertion failed: %s in %s at %zu\n", msg, file, line);
		exit(~0);
	}
}

void *ymalloc(size_t size)
{
	if(size == 0)
		size = 1;

	void *ptr = malloc(size);

	yassert(ptr != NULL, "memory allocation failed");

	return ptr;
}

void *ycalloc(size_t num, size_t size)
{
	if(size == 0)
		size = 1;
	
	void *ptr = calloc(num, size);
	
	yassert(ptr != NULL, "zero-initialized memory allocation failed");

	return ptr;
}

void *yrealloc(void *ptr, size_t size)
{
	if(size == 0)
		size = 1;

	ptr = realloc(ptr, size);
	
	yassert(ptr != NULL, "memory reallocation failed");

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

	yassert(ptr != NULL, "string duplication failed");

	strcpy(ptr, str);

	return ptr;
}

