#pragma once
#include <string>
#include "LinkedList.h"
using std::string;
class Attribute;

/* TextSpecParser is used to read in general purpose Text Files following the format
	attribute = value in each line.
	- Each text parser is associated with a file, and contain the dissected values of the file.
	- ANSI only
*/
/* The parser isn't aware of the underlying file content or order, but just organize things in a structure pending retriving
	- When same parameters are repeted, fire a warning and ignore the second declaration
	- When retriving, user has expetecd type, when type doesn't match, return NULL
*/
class TextSpecParser
{
public:
	TextSpecParser(const char* filePath);	// Expect Abs or cwd relative file path
	~TextSpecParser();

public:
	// Retrieve functions: Return the value if type matches, input expect NULL terminated
	// Return NULL if cannot find
	const int* GetInt(char* attributeName);
	const float* GetFloat(char* attributeName);
	const bool* GetBool(char* attributeName);
	const float* GetFloatArray(char* attributeName, unsigned int size);
	const char* GetString(char* attributeName);	// NULL terminated ANSI/ASCII/UTF-8 ASCII Page

	// Debug Use
	void PrintAttributeStructure();

private:
	// Parsing Function
	void ParseLine(char* data, unsigned int size); // And in the end it turns out I am more comfortable with bytes

	// Search Function, used internally, so name is not NULL terminated
	Attribute* FindAttribute(char* name, unsigned int length);
	// Helper Function
	bool StringEqual(const char* str1, const char* str2, unsigned int bytes);	// Return true if equal

private:
	// Datas
	LinkedList attributes;
};