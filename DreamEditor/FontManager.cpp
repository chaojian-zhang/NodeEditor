#define _CRT_SECURE_NO_WARNINGS
#include "FontManager.h"
#include "GraphManager.h"
#include <stdio.h>
#include <iostream>
#include <IL\il.h>
#include <IL\ilu.h>
#include "SystemWrapper.h"
using std::cout;
using std::cin;
using std::endl;

extern void CheckDevILError();

FontChar* FontManager::charCodes[65536] = { 0 };

FontManager::FontManager()
{
	// Load the font description file
	char pathBuffer[MAX_PATH];
	char* absPath = SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::CharMapPath, pathBuffer);
	FILE* font = fopen(absPath, "rb");	// Comment: strangely enough, VS is seeking dll in executable directory while current working dir for read is the project directory
	char blockIdentifier;
	int blockSize;


	// Debug_Temp: Require Better Way to do Error Check
	if (font == NULL)
	{
		cout << "[Error]Cannot Find Font file, application data is not complete. The application will still run but the display will go wrong." <<
			"Error At: " << __FILE__ << ", " << __LINE__ << endl;;
	}

	// Read format and version
	fread(&fileIdentifier, 1, 3, font);
	fread(&version, 1, 1, font);

	// Read font desciption
	// Create Infor Block
	fread(&blockIdentifier, 1, 1, font);
	fread(&blockSize, 1, 4, font);
	if (blockIdentifier != 1){ cout << "[Error]Encounter Error when read info block. At "<< __LINE__ << endl; };	// __DEBUG__: Require better solution
	info = new FontInfo(blockSize);
	// Read Infor block fields
	fread(&info->fontSize, 1, 2, font);
	fread(&info->bitField, 1, 1, font);
	fread(&info->charSet, 1, 1, font);
	fread(&info->stretchH, 1, 2, font);
	fread(&info->aa, 1, 1, font);
	fread(&info->paddingUp, 1, 1, font);
	fread(&info->paddingRight, 1, 1, font);
	fread(&info->paddingDown, 1, 1, font);
	fread(&info->paddingLeft, 1, 1, font);
	fread(&info->spacingHoriz, 1, 1, font);
	fread(&info->spaceVert, 1, 1, font);
	fread(&info->outline, 1, 1, font);
	fread(info->fontName, 1, blockSize - 14, font);
	// Debug_Temp
	cout << "Extracted Font Descirption as per: " << info->fontName << endl;

	// Create Common Block
	fread(&blockIdentifier, 1, 1, font);
	fread(&blockSize, 1, 4, font);
	if (blockIdentifier != 2){ cout << "[Error]Encounter Error when read common block. At " << __LINE__ << endl; };	// __DEBUG__: Require better solution
	common = new FontCommon();
	// Read Common block fields
	fread(&common->lineHeight, 1, 2, font);
	fread(&common->base, 1, 2, font);
	fread(&common->scaleW, 1, 2, font);
	fread(&common->scaleH, 1, 2, font);
	fread(&common->pages, 1, 2, font);
	fread(&common->bitField, 1, 1, font);
	fread(&common->alphaChannel, 1, 1, font);
	fread(&common->redChannel, 1, 1, font);
	fread(&common->greenChannel, 1, 1, font);
	fread(&common->blueChannel, 1, 1, font);
	// Debug_Temp
	cout << "Character map pages: " << common->pages << endl;

	// Create Pages Block
	fread(&blockIdentifier, 1, 1, font);
	fread(&blockSize, 1, 4, font);
	if (blockIdentifier != 3){ cout << "[Error]Encounter Error when read pages block. At " << __LINE__ << endl; };	// __DEBUG__: Require better solution
	pageNames = (char*)malloc(blockSize);	// pageNames is seriously used just as a void* buffer here. But we assign it char* type so compiler knows how to calculate byte size
	// Read Pages block fields
	char testChar;
	fread(&testChar, 1, 1, font);
	while (testChar != NULL)
	{
		pageNames[pageNameLength] = testChar;
		pageNameLength++;
		fread(&testChar, 1, 1, font);
	}
	pageNames[pageNameLength] = testChar;
	pageNameLength++;
	// Read in other names
	for (int i = 1; i < common->pages; i++)
	{
		fread(pageNames + i*pageNameLength, 1, pageNameLength, font);
	}
	// Debug_Temp
	cout << "Name of Last Character map: " << pageNames + (common->pages - 1)*pageNameLength << endl;

	// Create Character Block
	fread(&blockIdentifier, 1, 1, font);
	fread(&blockSize, 1, 4, font);
	if (blockIdentifier != 4){ cout << "[Error]Encounter Error when read pages block. At " << __LINE__ << endl; };	// __DEBUG__: Require better solution
	numCharacters = blockSize / 20;
	chars = new FontChar[numCharacters];
	for (int i = 0; i < numCharacters; i++)
	{
		// Read Character block fields
		fread(&chars[i].id, 1, 4, font);
		fread(&chars[i].x, 1, 2, font);
		fread(&chars[i].y, 1, 2, font);
		fread(&chars[i].width, 1, 2, font);
		fread(&chars[i].height, 1, 2, font);
		fread(&chars[i].xoffset, 1, 2, font);
		fread(&chars[i].yoffset, 2, 1, font);
		fread(&chars[i].xadvance, 1, 2, font);
		fread(&chars[i].page, 1, 1, font);
		fread(&chars[i].channel, 1, 1, font);
		// Record/Register in related field
		charCodes[chars[i].id] = &chars[i];
		// Range Check
		if (chars[i].id > 65535)
		{
			cout << "[Error]Chracter Codepoint out of range! At " << __LINE__ << endl; 	// __DEBUG__: Require better solution
		}
	}

	// Didn't read kerning pairs because we do not use them

	// Close file
	fclose(font);

	// Debug_Temp
	// Output useful information
	cout << ">>> Information about the font being loaded: " << endl;
	printf("\t %s with %i characters, outputed in %i %ix%i character maps.\n",
		info->fontName, numCharacters, common->pages,
		common->scaleW, common->scaleH);

	// Generate Charmaps
	GenerateCharMaps();
}

void FontManager::GenerateCharMaps()
{
	// Allocate textures
	textures = new GLuint[common->pages];

	// Generate OpenGL object
	glGenTextures(common->pages, textures);

	// Open all character maps
	for (unsigned int i = 0; i < common->pages; i++)
	{
		// Open an image using DevIL
		ILuint image; // The image name to return.
		ilGenImages(1, &image); // Grab a new image name.
		ilBindImage(image);
		char tempPath[256] = "";
		strcat(tempPath, "\\Data\\");
		strcat(tempPath, GetPageFileName(i));
		char pathBuffer[MAX_PATH];
		char* absPath = SystemFunctions::GetExecutableDirectoryPath(tempPath, pathBuffer);
		if (ilLoadImage(absPath) == IL_FALSE)
		{
			CheckDevILError();
		}
		// Do some error check
		ILint format = ilGetInteger(IL_IMAGE_FORMAT);
		ILint type = ilGetInteger(IL_IMAGE_TYPE);
		if (format != IL_LUMINANCE || type != IL_UNSIGNED_BYTE)
		{
			cout << "[Error]Character map are not specified format and type, in " << __FILE__ << " , line " << __LINE__; 	// __DEBUG__: Require better solution
		}

		// Load the image into openGL
		int width = ilGetInteger(IL_IMAGE_WIDTH);
		int height = ilGetInteger(IL_IMAGE_HEIGHT);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, width, height);
		unsigned char* imageData = ilGetData();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);
		// Comment: How DEvIL load images:
		// As verified per IL_LUMINANCE, IL_UNSINGED_BYTE type image: the image has only one equivalent openGL red channel, and 255 represent full strength, while 127 is 0.5 value.
		// DevIl load images from UpperLeft corner to lower-right corner row by row, so is the arrangemeng of imageData: that is, element 4097 is the first column in the second row

		// Set Texture Mipmaps
		// Pending

		// Set Texture Sampling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Delete the image because not useful anymore
		ilDeleteImage(image);
	}
}

FontManager::~FontManager()
{
	// Delete Font specification related data
	delete info;
	delete common;
	delete chars;

	// Delete Other resources
	delete[] pageNames;
	delete[] textures;
}

char* FontManager::GetPageFileName(int pageIndex)
{
	if (pageIndex <= common->pages - 1)
	{
		return pageNames + pageIndex* pageNameLength;
	}
	else
	{
		return NULL;
	}
}
FontChar* FontManager::GetChar(unsigned int charCode)
{
	return charCodes[charCode];
}
GLuint FontManager::GetCharMap(unsigned int index)
{
	if (index <= common->pages)
	{
		return textures[index];
	}
	
	// __Debug__: Pending Better solution
	cout << "[Error]Requested CharMap doens't exist!" << endl;
	return NULL;
}