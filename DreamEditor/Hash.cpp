#include "Hash.h"

unsigned int
Hash::HashID(const unsigned char* string, unsigned int hashRange)
{
	unsigned long hash = 5381;
	int c;

	while (c = *string++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash % hashRange;
}
