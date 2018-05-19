#pragma once
#include <gl\glew.h>
#include "FontInfo.h"
#include "FontChar.h"
#include "FontCommon.h"

/*Font Manger is the central management class for loading and keeping all information about the font
we support and related font atlas*/
class FontManager
{
public:
	// Load and initialize font information
	FontManager();
	~FontManager();	// Free allocated memory

	char* GetPageFileName(int pageIndex);	// Not used by no one actually 
	FontChar* GetChar(unsigned int charCode);
	GLuint GetCharMap(unsigned int index);

	void GenerateCharMaps();

public:
	// Public access type for easier access
	FontInfo* info;
	FontCommon* common;
	FontChar* chars;	// Array of chars

private:
	// Not used
	int fileIdentifier;
	char version;

	// Pages
	int pageNameLength = 0;
	char* pageNames = 0;	// An array of pageNames, used as a void*

	// Characters
	int numCharacters = 0;
	static FontChar* charCodes[65536];	// ID range might not be complete so this way we do not need to guess the location of FontChar
	GLuint* textures;	// Array of textures
};