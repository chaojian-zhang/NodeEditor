#include "Time.h"
#include <stdlib.h>
#include <wchar.h>	// Warning Portability since we are using unsigned short*

unsigned short Time::timeString[20] = {};

unsigned short* Time::GetString()
{
	//swprintf((wchar_t*)timeString, 19+1, L"%0.4u-%02.2u-%02.2u %02.2u:%02.2u:%02.2u",
	swprintf((wchar_t*)timeString, 16 + 1, L"%0.4u-%02.2u-%02.2u %02.2u:%02.2u",
		(unsigned int)year, (unsigned int)month, (unsigned int)day, 
		(unsigned int)hour, (unsigned int)minute, (unsigned int)second);	// Automatically append a NULL
	return timeString;
}

unsigned int Time::GetStringLength()
{
	return 16 + 1;
}