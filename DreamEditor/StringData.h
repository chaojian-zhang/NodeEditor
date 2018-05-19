#pragma once
#include <stdlib.h>	// Provide Definition for NULL
#include "Time.h"

// Fixed Size Allocation Definitions to help eliminate calls to malloc, measured in wchar_t(ushort) units
// This will sacrifice some memory and gain a lot run-time efficiency
#define MAX_STRING_SIZE 65536	
#define LeveledStringLength_0 128
#define LeveledStringLength_1 256
#define LeveledStringLength_2 512
#define LeveledStringLength_3 2048
#define LeveledStringLength_4 8192
#define LeveledStringLength_5 32768
#define LeveledStringLength_6 MAX_STRING_SIZE

/* String Data communication:
	Strins have two sources: user-typed string, which is isoloated unsigned short values; 
	and pre-defiend interface strings, which end with NULL terminator.
*/
class StringData
{
public:
	// StringData(unsigned short* initialString);	// Fixed Size as terminating with NULL; Theoratically and practically, we never use this constructor since all static strings are directly feeded to renderables
	StringData(unsigned short* initialString, unsigned int numChars);	// Content Initialized String, auto fit to proper size
	StringData(); // First Level Dynamic Allocated String
	~StringData();	// Release memory

	// String Data Manipulation, provide dynamic expansion
	void AddToString(unsigned int cursorNumChar, unsigned short* insertString, unsigned int numChars);
	// Provide dynamic-expansion insertion functionality, can
	// also be used as deletion function. This is to avoid have to delete first then insert
	void ReplaceSubString(unsigned int cursorNumChar1, unsigned int cursorNumChar2, unsigned short* insertString, unsigned int numChars);
	// Deletion functions
	void DeleteSubString(unsigned int cursorNumChar1, unsigned int cursorNumChar2);
	bool DeleteForward(unsigned int cursorNumChar);	// Towards end/right;	Return whether or not we delete anything
	bool DeleteBackward(unsigned int cursorNumChar);	// Towards beginning/left, because this is trigger by "backspace" button

	// Facility
	void Export(const char* name, Time* timePrefix = NULL);	// We will append a suffix to the pure name

	// State Queries
	unsigned short* GetString();	// String is NOT NULL terminated
	unsigned int GetStringNumChars();

	static unsigned short copyBuffer[65536];	// Used to handle RelaceSubString() special case and AddToString()

private:
	unsigned int stringNumChars;
	unsigned short* string;	// String is NOT NULL terminated

private:
	unsigned int stringPreNumChars = LeveledStringLength_0;	// Preallocated Size
	void UpdateStringLevel(unsigned int addition = 0);
	void ReallocateString(unsigned int newSize);
};