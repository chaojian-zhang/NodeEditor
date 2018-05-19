#include "FontInfo.h"

FontInfo::FontInfo(int blockSize)
{
	fontName = (char*)malloc(blockSize - 14);
}

FontInfo::~FontInfo()
{
	free(fontName);
}