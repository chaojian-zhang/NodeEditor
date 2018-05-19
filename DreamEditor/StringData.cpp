#define _CRT_SECURE_NO_WARNINGS
#include "StringData.h"
#include "MathElegent.h"
#include "SystemWrapper.h"
#include <string.h>	// memcpy
#include <iostream>
#include "Locales.h"

// Some problem I am having here is using int to convert unsigned int, although very unlikely, when we are dealing with really big unsigned int, this will cause accuracy problem

/*
StringData::StringData(unsigned short* initialString)
{
	numChars = GetStringLength(initialString);
	string = new unsigned short[numChars];
	memcpy(string, initialString, numChars * 2);	// 2 bytes each; Obviously, string is not NULL terminated
}*/

// Static Variable
unsigned short StringData::copyBuffer[65536] = { 0 };

StringData::StringData(unsigned short* initialString, unsigned int numChars)
{
	// Record Information
	stringNumChars = numChars;

	// Create a default stirng container and upgrade to a proper size
	string = new unsigned short[LeveledStringLength_0]();
	UpdateStringLevel();

	// Copy the actual data safely now
	memcpy(string, initialString, stringNumChars * 2);
}

StringData::StringData()
{
	// Record Information
	stringNumChars = 0;

	// Create a default stirng container with first level string size
	string = new unsigned short[LeveledStringLength_0]();
}

StringData::~StringData()
{
	delete[] string;	// string is an array so we need to use delete[]
}

// String Manipulation
// Accept All Text Characters + Tab/NewLine
// : Notice we didn't strip out any invalid characters like \r but to let them reside in the memory (e.g. when paste texts from other text editors)
void StringData::AddToString(unsigned int cursorNumChar, unsigned short* insertString, unsigned int numChars)
{
	// First Check Update
	UpdateStringLevel(numChars);

	// Assertion
	if (stringNumChars >= cursorNumChar)
	{
		// Copy Trailing String to copyBuffer
		memcpy(copyBuffer, string + cursorNumChar, (stringNumChars - cursorNumChar)*sizeof(unsigned short));	// From numChars to bytes need to multiply by 2

		// Copy Insertion String
		memcpy(string + cursorNumChar, insertString, numChars *sizeof(unsigned short));

		// Copy Trailing String Back
		memcpy(string + cursorNumChar + numChars, copyBuffer, (stringNumChars - cursorNumChar) *sizeof(unsigned short));

		// Update stringNumChar
		stringNumChars += numChars;
	}

}

void StringData::ReplaceSubString(unsigned int cursorNumChar1, unsigned int cursorNumChar2, unsigned short* insertString, unsigned int numChars)
{
	unsigned int frontCursor = Mini(cursorNumChar1, cursorNumChar2);
	unsigned int backCursor = Maxi(cursorNumChar1, cursorNumChar2);
	
	// __Debug__: Pending Verifying the legitity of this warning-less assertion. It does seem harmless and sort of expected by the user of this function.
	// Assertion
	if (stringNumChars >= frontCursor && stringNumChars >= backCursor)
	{
		unsigned int deleteNumChars = backCursor - frontCursor;
		// Check String Preallocated Length update
		UpdateStringLevel(abs((int)deleteNumChars - (int)numChars));

		if (numChars <= deleteNumChars)
		{
			// Copy Insertion String
			memcpy(string + frontCursor, insertString, numChars*sizeof(unsigned short));

			// Copy Trailing String
			memcpy(string + frontCursor + numChars, string + backCursor, stringNumChars - backCursor);	// Doens't matter if there is an overlap, as long as memcpy goes in a linear order

			// Overide the last characters with 0
			unsigned int replacedNumChars = deleteNumChars - numChars;
			stringNumChars -= replacedNumChars;
			memset(string + stringNumChars, 0, sizeof(unsigned short) * replacedNumChars);
		}
		else // We gotta handle this specially
		{
			// __Debug__: Assertion
			if (stringNumChars - backCursor < 65536)
			{
				// Copy Trailing String into copyBuffer first, to avoid being overlaped by Insertion String
				memcpy(copyBuffer, string + backCursor, (stringNumChars - backCursor)*sizeof(unsigned short));

				// Copy Insertion String
				memcpy(string + frontCursor, insertString, numChars*sizeof(unsigned short));

				// Copy Trailing String Back
				memcpy(string + frontCursor + numChars, copyBuffer, (stringNumChars - backCursor)*sizeof(unsigned short));

				// Update Numcharacters
				stringNumChars += numChars - deleteNumChars;
			}
		}
	}
}

void StringData::DeleteSubString(unsigned int cursorNumChar1, unsigned int cursorNumChar2)
{
	unsigned int frontCursor = Mini(cursorNumChar1, cursorNumChar2);
	unsigned int backCursor = Maxi(cursorNumChar1, cursorNumChar2);

	// Assertion
	if (stringNumChars >= frontCursor && stringNumChars >= backCursor)
	{
		// Copy Trailing String
		memcpy(string + frontCursor, string + backCursor, (stringNumChars - backCursor)*sizeof(unsigned short));

		// Overide the last characters with 0
		unsigned int deleteNumChars = backCursor - frontCursor;
		stringNumChars -= deleteNumChars;
		memset(string + stringNumChars, 0, sizeof(unsigned short) * deleteNumChars);
	}
}

bool StringData::DeleteForward(unsigned int cursorNumChar)
{
	// Assertion
	if (stringNumChars > cursorNumChar)
	{
		// Copy Trailing String
		memcpy(string + cursorNumChar, string + cursorNumChar + 1, (stringNumChars - (cursorNumChar + 1))*sizeof(unsigned short));

		// Overide the last character with 0
		stringNumChars--;
		memset(string + stringNumChars, 0, 2);
		return true;
	}

	return false;
}

bool StringData::DeleteBackward(unsigned int cursorNumChar)
{
	// Assertion
	if (stringNumChars > 0 && stringNumChars >= cursorNumChar && cursorNumChar != 0)
	{
		// Copy Trailing String
		memcpy(string + cursorNumChar - 1, string + cursorNumChar, (stringNumChars - cursorNumChar)*sizeof(unsigned short));

		// Overide the last character with 0
		stringNumChars--;
		memset(string + stringNumChars, 0, 2);

		return true;
	}

	return false;
}

// Facility
void StringData::Export(const char* name, Time* timePrefix)
{
	// Generate real file name from pure file name
	static const char* suffix = ".txt";
	unsigned int size = strlen(name) + strlen(suffix) + 1;
	char* filename = (char*)malloc(size);
	strcpy(filename, name);
	strcat(filename, suffix);
	filename[size - 1] = 0;

	// Write to file in UTF-8
	FILE* file = fopen(filename, "w");

	// Write time header if any
	if (timePrefix)
	{
		// Convert time Content
		// Prefix
		unsigned short* originalPrefix = (unsigned short*)TextFile_Export_CreateTime;
		unsigned int prefixStringLength = GetStringLength(originalPrefix);

		unsigned int prefixStringSize = SystemFunctions::U16ToU8(originalPrefix, prefixStringLength);
		char* prefixString = (char*)malloc(prefixStringSize);
		SystemFunctions::U16ToU8(originalPrefix, prefixStringLength, prefixString, prefixStringSize);

		fwrite(prefixString, prefixStringSize, 1, file);
		free(prefixString);

		// Time
		unsigned short* originalTime = timePrefix->GetString();
		unsigned int timeStringLength = timePrefix->GetStringLength() - 1;

		unsigned int timeStringSize = SystemFunctions::U16ToU8(originalTime, timeStringLength);	// No need to +2 since we do not wish to be NULL terminated in this case
		char* timeString = (char*)malloc(timeStringSize);
		SystemFunctions::U16ToU8(originalTime, timeStringLength, timeString, timeStringSize);

		fwrite(timeString, timeStringSize, 1, file);		
		free(timeString);

		// Newline
		unsigned short* originalNewline = (unsigned short*)TextFile_Export_Aux_Newline;
		unsigned int newlineStringLength = GetStringLength(originalNewline);

		unsigned int newlineStringSize = SystemFunctions::U16ToU8(originalNewline, newlineStringLength);
		char* newlineString = (char*)malloc(newlineStringSize);
		SystemFunctions::U16ToU8(originalNewline, newlineStringLength, newlineString, newlineStringSize);

		fwrite(newlineString, newlineStringSize, 1, file);
		free(newlineString);
	}
	// Write actual text content
	// Convert String Content
	unsigned int contentSize = SystemFunctions::U16ToU8(string, stringNumChars);	// No need to +2 since we do not wish to be NULL terminated in this case
	char* filecontent = (char*)malloc(contentSize);
	SystemFunctions::U16ToU8(string, stringNumChars, filecontent, contentSize);
	fwrite(filecontent, contentSize, 1, file);

	// Close the file
	fclose(file);

	// Release Resources
	free(filename);
	free(filecontent);
}

// Helper FUnctions
unsigned short* StringData::GetString()
{
	return string;
}

unsigned int StringData::GetStringNumChars()
{
	return stringNumChars;
}

void StringData::UpdateStringLevel(unsigned int addition)
{
	unsigned int requiredNumChars = stringNumChars + addition;

	if (requiredNumChars > stringPreNumChars)
	{
		if (requiredNumChars < LeveledStringLength_0)
		{
			ReallocateString(LeveledStringLength_0);
		}
		else if (requiredNumChars < LeveledStringLength_1)
		{
			ReallocateString(LeveledStringLength_1);
		}
		else if (requiredNumChars < LeveledStringLength_2)
		{
			ReallocateString(LeveledStringLength_2);
		}
		else if (requiredNumChars < LeveledStringLength_3)
		{
			ReallocateString(LeveledStringLength_3);
		}
		else if (requiredNumChars < LeveledStringLength_4)
		{
			ReallocateString(LeveledStringLength_4);
		}
		else if (requiredNumChars < LeveledStringLength_5)
		{
			ReallocateString(LeveledStringLength_5);
		}
		else if (requiredNumChars < LeveledStringLength_6)
		{
			ReallocateString(LeveledStringLength_6);
		}
		else
		{
			ReallocateString(requiredNumChars);
			std::cout << "[Warning] Caution string length is quite long: " << requiredNumChars << std::endl;
		}

		/*
		// Previous vversion:
		//		Increment string size level works fine for manual input but doesn't suit when paste huge amount of text
		switch (stringPreNumChars)
		{
		case LeveledStringLength_0:
			ReallocateString(LeveledStringLength_1);
			break;
		case LeveledStringLength_1:
			ReallocateString(LeveledStringLength_2);
			break;
		case LeveledStringLength_2:
			ReallocateString(LeveledStringLength_3);
			break;
		case LeveledStringLength_3:
			ReallocateString(LeveledStringLength_4);
			break;
		case LeveledStringLength_4:
			ReallocateString(LeveledStringLength_5);
			break;
		case LeveledStringLength_5:
			ReallocateString(LeveledStringLength_6);
			break;
		case LeveledStringLength_6:	// The Theoratical Max String Size we could address is: range(unsigned interger)/2, which would be 2^32 / 2 = 2147483648 ushort characters, which would be 4 terabytes
			ReallocateString(2147483648/2 -1);	// Devide by two here to compensate (unsigned int) to (int) conversion we used in our functions
			break;
		default:	// Only when we are above Level 6 this could occur
			std::cout << "[Error] Serious error occured when try to manage string Data: \
						 The reqeusted string data range is above 4GB. \
						 Please make sure you are not creating too many rambling characters." 
						 << std::endl;
		}*/
	}
}

void StringData::ReallocateString(unsigned int newSize)
{
	unsigned short* oldString = string;
	stringPreNumChars = newSize;
	string = new unsigned short[stringPreNumChars]();	// use new() to initialize to 0
	memcpy(string, oldString, stringNumChars * 2);
	delete[] oldString;
}


