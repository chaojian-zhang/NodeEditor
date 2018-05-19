#include "Attribute.h"
#include <stdlib.h>
#include <string.h>
// Debug USe
#include <iostream>
using std::cout;
using std::endl;

Attribute::Attribute(unsigned int count, ElementType type, unsigned int stringContentSize)
{
	// Save information
	arrayLength = count;
	elementType = type;
	
	// Allocate Space for Storage
	switch (type)
	{
	case Integer:
		values = (char*)malloc(sizeof(int)*count);
		break;
	case Float:
		values = (char*)malloc(sizeof(float)*count);
		break;
	case Boolean:
		values = (char*)malloc(sizeof(bool)*count);
		break;
	case String:
		values = (char*)calloc(1, stringContentSize + count);	// count for NULL terminators
		break;
	default:
		values = NULL;
		break;
	}
}

Attribute::~Attribute()
{
	if(values)free(values);
	if (attributeName) free(attributeName);
}

void Attribute::AssignAttributeName(char* name, unsigned int size)
{
	attributeName = (char*)calloc(1, size +1);
	memcpy(attributeName, name, size);
}

void Attribute::AddElement(char* value, unsigned int size)
{
	switch (elementType)
	{
	case Integer:
		*(int*)(values + elementsSize) = StringToInt(value, size);
		elementsSize+=sizeof(int);
		break;
	case Float:
		*(float*)(values + elementsSize) = StringToFloat(value, size);
		elementsSize += sizeof(float);
		break;
	case Boolean:
		*(bool*)(values + elementsSize) = StringToBool(value, size);
		elementsSize += sizeof(bool);
		break;
	case String:
		if (*value == '\"')	// This means we are treating real strings
		{
			memcpy(values + elementsSize, value + 1, size - 2);
			elementsSize += size - 2 + 1;	// 1 for NULL terminator
		}
		else
		{
			// This means we are strongly treat non-strings as strings
			memcpy(values + elementsSize, value, size);
			elementsSize += size + 1; 	// 1 for NULL terminator
		}
		break;
	default:
		break;
	}
}

const char* Attribute::GetName()
{
	return attributeName;
}

ElementType Attribute::GetType()
{
	return elementType;
}

unsigned int Attribute::GetArrayLength()
{
	return arrayLength;
}

char* Attribute::GetValues()
{
	return values;
}

void Attribute::Print()
{
	unsigned int printedByes = 0;

	cout << "Attribute::" << attributeName << " ";
	switch (elementType)
	{
	case Integer:
		cout << "<Int>: ";
		break;
	case Float:
		cout << "<Float>: ";
		break;
	case Boolean:
		cout << "<Bool>: ";
		break;
	case String:
		cout << "<String>: ";
		break;
	default:
		break;
	}
	cout << "{";
	for (unsigned int i = 0; i < arrayLength; i++)
	{
		switch (elementType)
		{
		case Integer:
			cout << *((int*)values + i);
			break;
		case Float:
			cout << *((float*)values + i);
			break;
		case Boolean:
			cout << (*((bool*)values + i) ? "true" : "false");
			break;
		case String:
			cout << "\"" << values + printedByes << "\"";
			printedByes += strlen(values + printedByes) + 1;	// 1 for NULL terminator
			break;
		default:
			break;
		}

		// If we are not the last element, then print a "," before the next elemeent
		if(i != arrayLength - 1)
			cout << ", ";
	}
	cout << "}";
	cout << endl;
}

int Attribute::StringToInt(char* string, unsigned int size)
{
	char buffer[64] = {0};
	memcpy(buffer, string, size);
	return atoi(buffer);
}

float Attribute::StringToFloat(char* string, unsigned int size)
{
	char buffer[64] = { 0 };
	memcpy(buffer, string, size);
	float temp = atof(buffer);
	return (float)atof(buffer);
}

bool Attribute::StringToBool(char* string, unsigned int size)
{
	char buffer[64] = { 0 };
	memcpy(buffer, string, size);
	// return strstr(string, "true");
	return strstr(buffer, "true");
}