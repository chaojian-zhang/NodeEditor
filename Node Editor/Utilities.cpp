#include "Utilitites.h"
#include <wchar.h>

// Didn't use a log output becase opening filename is still locale specific if we want to use Unicode output, so it is much cleaner to just use Console output for errors
void ErrorOutput(const char* string, int line_nb)	// Char* string here for ANSI console output
{
	printf("Error: %ls, at line: %i\n", string, line_nb);
	__debugbreak();
	exit(EXIT_FAILURE);
}