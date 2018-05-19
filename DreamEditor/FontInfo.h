#pragma once
#include <stdlib.h>
class FontInfo
{
public:
	FontInfo(int blockSize);	// Use blockSize to allocate char*
	~FontInfo();	// Free the memory allocated for char*
public:
	short fontSize;
	char bitField;
	unsigned char charSet;
	unsigned short stretchH;
	unsigned char aa;
	unsigned char paddingUp;
	unsigned char paddingRight;
	unsigned char paddingDown;
	unsigned char paddingLeft;
	unsigned char spacingHoriz;
	unsigned char spaceVert;
	unsigned char outline;
	char* fontName;	// Terminate will null
};