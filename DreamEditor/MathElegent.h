#pragma once

// Minimum-Sized Small Math utilities; Named so to avoid collision with standard libraris
int Maxi(int a, int b);

int Mini(int a, int b);

float Maxf(float a, float b);

float Minf(float a, float b);

// Aloso provide some useful data structures
struct Vector3f
{
	float x;
	float y;
	float z;
};

// Return the length of string, dosn't include NULL character
// The complete storage space for a string should be GetStringLength(string)*2 + 2
unsigned int GetStringLength(unsigned short* string);

// bool CompareStrings(unsigned short* string1, unsigned short* string2);	// Compare U16 Strings, equal return true, assume NULL terminated