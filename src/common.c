
#include "common.h"

void *ymalloc(size_t bytes)
{
	if(bytes == 0)
		bytes = 1;

	void *ptr = malloc(bytes);
	if(ptr == NULL)
	{
		fputs("memory allocation failed\n", stderr);
		exit(~0x0);
	}

	return ptr;
}

void yfree(void *ptr)
{
	if(ptr != NULL)
		free(ptr);
}

