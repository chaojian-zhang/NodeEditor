#include "MathElegent.h"

// Minimum-Sized Small Math utilities; Named so to avoid collision with standard libraris
int Maxi(int a, int b)
{
	return a > b ? a : b;
}
int Mini(int a, int b)
{
	return a < b ? a : b;
}

float Maxf(float a, float b)
{
	return a > b ? a : b;
}
float Minf(float a, float b)
{
	return a < b ? a : b;
}

// Return the length of string, dosn't include NULL character
// The complete storage space for a string should be GetStringLength(string)*2 + 2
unsigned int GetStringLength(unsigned short* string)
{
	if (string)
	{
		unsigned int length = 0;

		// Read a unsigned short character untill we meet NULL
		unsigned short* oneChar = string;
		while (*oneChar != 0)	// Equivalent as (oneChar != 0)
		{
			oneChar++;
			length++;
		}

		return length;
	}
	else
	{
		return 0;
	}

}