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


//bool CompareStrings(unsigned short* string1, unsigned short* string2)
//{
//	if (string1 == string2)
//	{
//		return true;
//	}
//
//	unsigned int stringLength = GetStringLength(string1);
//	if (stringLength != GetStringLength(string2))
//	{
//		return false;
//	}
//
//	for (unsigned int i = 0; i < stringLength; i++)
//	{
//		if (string1[i] != string2[i])
//		{
//			return false;
//		}
//	}
//}