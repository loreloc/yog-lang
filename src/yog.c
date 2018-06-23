
#include "scanner.h"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("usage:\tyog <source-filename>\n"); // TODO: check for .yog file type
		return -1;
	}

	const char *source_filename = argv[1];

	// open the source file
	FILE *source_file = fopen(source_filename, "r");
	if(!source_file)
	{
		printf("failed to open %s\n", source_filename);
		return 1;
	}

	// initialize the scanner
	scanner_t scanner;
	scanner_init(&scanner, source_file);

	// TODO: lexical analysis

	// close the source file
	fclose(source_file);

	return 0;
}

