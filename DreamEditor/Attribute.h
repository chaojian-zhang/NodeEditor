#pragma once
enum ElementType
{
	Integer,
	Float,
	Boolean,
	String
};


/* This class is used by TextSpecParser, in order to properly store an attribute*/
class Attribute
{
public:
	Attribute(unsigned int count, ElementType type, unsigned int stringContentSize);	// Initialize to default in case some elements are not assigned
	// stringContentSize should be the number of characters in total
	~Attribute();

	void AssignAttributeName(char* name, unsigned int size);
	void AddElement(char* value, unsigned int size);
	const char* GetName();
	ElementType GetType();
	unsigned int GetArrayLength();
	char* GetValues();

	// Debug Use
	void Print();

private:
	int StringToInt(char* string, unsigned int size);
	float StringToFloat(char* string, unsigned int size);
	bool StringToBool(char* string, unsigned int size);

private:
	// Every thing is a first class array
	char* attributeName = 0;
	unsigned int arrayLength = 0;	// The total number expected to be add in
	char* values;
	ElementType elementType;

	// bookkeeper
	unsigned int elementsSize = 0;	// Those already added in, in bytes, not the number of elements
};