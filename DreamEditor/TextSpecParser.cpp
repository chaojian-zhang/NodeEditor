#define _CRT_SECURE_NO_WARNINGS
#include "TextSpecParser.h"
#include <stdio.h>
#include "LinkedList.h"
#include "Attribute.h"

TextSpecParser::TextSpecParser(const char* filePath)
{
	// Open The File
	FILE* file = fopen(filePath, "rb");

	// Error Check
	if (!file)
	{
		perror("[Error] Cannot Open File for Parse.");
		__debugbreak();
	}

	// Get Content Size
	fseek(file, 0L, SEEK_END);
	unsigned int fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	// Allocate Enough Memoery For processing purpose
	char* tempFileBuffer = (char*)calloc(1, fileSize);
	char tempChar = 0;
	unsigned int lineSize = 0;
	bool bInsideQuote = false;
	bool bInsideComment = false;
	bool bInsideCommentBlock = false;
	
	// Strip Out all white spaces and store in memory
	// And in the end it turns out I am more comfortable with bytes
	while (!feof(file))
	{
		// Read a character for inspection
		fread(&tempChar, 1, 1, file);
		if (feof(file)) break;

		// Top-Level Key Character: If this is a comment block, switch on Comment Block boolean
		if (tempChar == '*')
		{
			// A single * with out a / can also begin a comment block
			bInsideCommentBlock = true;
			lineSize = 0;	// Just to be safe and avoid bugs, we set this to 0 although logically/ideal this should already be zero at this time

			// Read a character to check whether or not to switch off comment block: if it is a / after this * we will switch off, otherwise it doesn't matter since we are anyway ignoring texts
			fread(&tempChar, 1, 1, file);
			if (tempChar == '/')
			{
				bInsideCommentBlock = false;
			}
			continue;
		}

		// Only Parse when not inside a comment block, otherwise just ignore everything
		if (!bInsideCommentBlock)
		{
			// If it is a { or a }, strip it out and continue
			if (tempChar == '}' || tempChar == '{' || tempChar == ';' || tempChar == '\r')
			{
				continue;
			}

			// If it is a space not inside a quote, then strip it out and continue
			if (tempChar == ' ' && !bInsideQuote)
			{
				continue;
			}

			// If a quote, then switch quote boolearn
			if (tempChar == '\"')
			{
				bInsideQuote = !bInsideQuote;
			}

			// If it is a comment and we are not inside a comment block(when inside a comment block, // is ignored), then mark we are in a comment so newline get passed, and process what we have got now
			if (tempChar == '/')
			{
				bInsideComment = true;

				if (lineSize != 0)	// In case of trailing \n lines without content
				{
					ParseLine(tempFileBuffer, lineSize);
					lineSize = 0;
				}

				continue;
			}

			// If it is a new line and we are not in a comment, then process what we have got, and then continue
			if (tempChar == '\n')
			{
				// If we are in a comment, meaning the useful content of the line has already been parsed
				if (bInsideComment)
				{
					// Continue Next Line without parsing anything
					bInsideComment = false;
					lineSize = 0;
					continue;
				}

				// Other wise we need to parse the line and then continue
				if (lineSize != 0)	// In case of trailing \n lines without content
					ParseLine(tempFileBuffer, lineSize);

				// Continue Next Line
				lineSize = 0;
				continue;
			}

			if (bInsideComment) continue;

			// Otherwise store the character in memory
			tempFileBuffer[lineSize] = tempChar;
			lineSize++;
		}
		else
		{
			continue;
		}
	}
	if (lineSize != 0)	// Handle the case when we didn't have a new line character for the last attribute
	{
		ParseLine(tempFileBuffer, lineSize);
	}

	// Release Memory
	free(tempFileBuffer);

	// Close the file
	fclose(file);
}

TextSpecParser::~TextSpecParser()
{
	// Deallocate All Attributes
	Attribute* attr = (Attribute*)(attributes.First());
	while (attr)
	{
		delete attr;
		attr = (Attribute*)(attributes.Next());
	}
}

const int* TextSpecParser::GetInt(char* attributeName)
{
	Attribute* attr = FindAttribute(attributeName, strlen(attributeName));
	if (attr && attr->GetType() == ElementType::Integer && attr->GetArrayLength() == 1)
	{
		return (int*)(attr->GetValues());
	}
	
	return NULL;
}

const float* TextSpecParser::GetFloat(char* attributeName)
{
	static float tempFloat;

	Attribute* attr = FindAttribute(attributeName, strlen(attributeName));
	if (attr && (attr->GetType() == ElementType::Float/* || attr->GetType() == ElementType::Integer, sometimes direct conversion(use static cast as below) isn't possible e.g. Int 128 isn't a legit float*/) && attr->GetArrayLength() == 1)	// Treat an int as float as well, ideally -> In this implementation we didn't actually treat an int as float, as said in the comment
	{
		return (float*)(attr->GetValues());
	}
	// But we cannot let the user suffer
	if (attr && attr->GetType() == ElementType::Integer && attr->GetArrayLength() == 1)
	{
		int value = *((int*)(attr->GetValues()));	// Use (int*) to avoid it being interpreted as char* which is only 1byte long
		tempFloat = value;	// Do an explicit conversion rather than static reinterpreration
		return &tempFloat;
	}

	return NULL;
}

const bool* TextSpecParser::GetBool(char* attributeName)
{
	Attribute* attr = FindAttribute(attributeName, strlen(attributeName));
	if (attr && attr->GetType() == ElementType::Boolean && attr->GetArrayLength() == 1)
	{
		return (bool*)(attr->GetValues());
	}

	return NULL;
}

const float* TextSpecParser::GetFloatArray(char* attributeName, unsigned int size)
{
	Attribute* attr = FindAttribute(attributeName, strlen(attributeName));
	if (attr && attr->GetType() == ElementType::Float && attr->GetArrayLength() == size)
	{
		return (float*)(attr->GetValues());
	}

	return NULL;
}

const char* TextSpecParser::GetString(char* attributeName)
{
	Attribute* attr = FindAttribute(attributeName, strlen(attributeName));
	if (attr && attr->GetType() == ElementType::String && attr->GetArrayLength() == 1)
	{
		return (char*)(attr->GetValues());
	}

	return NULL;
}

void TextSpecParser::PrintAttributeStructure()
{
	Attribute* attr = (Attribute*)(attributes.First());
	while (attr)
	{
		attr->Print();
		attr = (Attribute*)(attributes.Next());
	}
}

// Basic Format: attribtuename = {value, more values}\n -- newline, spaces, and {} are stripped if any
void TextSpecParser::ParseLine(char* data, unsigned int size)
{
	// Temp Variables
	char attributeNameBuffer[256] = {0};
	unsigned int nameLength = 0;

	// Process each character and get what we want
	for (unsigned int i = 0; i < size; i++)
	{
		// Read as attribute name until encounter = sign
		if (data[i] == '=')
		{
			nameLength = i;
			break;
		}
		else
		{
			attributeNameBuffer[i] = data[i];
			continue;
		}
	}

	// Error Handling
	if (nameLength == 0)	// Cannot find an qual sign
	{
		// Meaning there is no assignment in the statement
		// Then we ignore this statement
		return;
	}
	if (FindAttribute(attributeNameBuffer, nameLength))	// If attribute already exist
	{
		// Then we only keep the first one, and ignore repeated definitions
		return;
	}

	// Get Number of Array Elements and Element Type
	unsigned int elementCount = 1;	// Assume a blank value as one
	bool bFloat = false; // Assume Consistent Type
	bool bString = false; // Assume Consistent Type
	bool bBoolean = false; // Assume Consistent Type
	unsigned int quoteCount = 0;
	for (unsigned int i = nameLength + 1; i < size; i++)
	{
		if (data[i] == ',')
		{
			elementCount++;
			continue;
		}

		if (data[i] == '\"')
		{
			bString = true;
			quoteCount++;
			continue;
		}

		if (data[i] == '.')
		{
			bFloat = true;
			continue;
		}

		if (data[i] == 't' || data[i] == 'f')
		{
			bBoolean = true;
			continue;
		}
	}

	// Create and Initialize Attribute Object
	Attribute* newAttribute = new Attribute(elementCount, 
		bString ? ElementType::String : 
		(bBoolean ? ElementType::Boolean :
		(bFloat ? ElementType::Float : ElementType::Integer)), bString?(size - elementCount - quoteCount):0); // String has priority type than boolearn than float than integer

	// Assign Attribute Name
	newAttribute->AssignAttributeName(attributeNameBuffer, nameLength);

	// Contine read elements
	unsigned int dataSize = 0;
	for (unsigned int i = nameLength + 1; i < size; i++)
	{
		// Read Data Until Encounter an Comma
		if (data[i] == ',')
		{
			newAttribute->AddElement(data + i - dataSize, dataSize);

			dataSize = 0;
			continue;
		}
		
		dataSize++;
	}

	// In case trailing element of the array or we have only one element and there is no comma
	if (dataSize != 0 || size == nameLength + 1)
	{
		newAttribute->AddElement(data + size - dataSize, dataSize);
	}

	attributes.Add(newAttribute);
}

Attribute* TextSpecParser::FindAttribute(char* name, unsigned int length)
{
	Attribute* found = NULL;

	Attribute* attr = (Attribute*)(attributes.First());
	while (attr)
	{
		// The two names must be an exact match, not just a substring match, that's why we need to compare length first
		if (length == strlen(attr->GetName()) && StringEqual(attr->GetName(), name,length))
		{
			found = attr;
			break;
		}
		attr = (Attribute*)(attributes.Next());
	}

	return found;
}

bool TextSpecParser::StringEqual(const char* str1, const char* str2, unsigned int bytes)
{
	for (unsigned int i = 0; i < bytes; i++)
	{
		if (str1[i] != str2[i]) return false;
	}
	return true;
}

// At first I was thinking about using string or stringstream etc for easier text, number differentiaion processing, but
// it turns out that it is much easier and reasonable to simply process things byte by byte so that we do not need to care about other
// librari'es behavior and read much documentations. And this way makes me feel safer. And typing is more smooth and fluent thus faster.