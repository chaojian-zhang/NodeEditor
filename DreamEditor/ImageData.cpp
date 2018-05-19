#define _CRT_SECURE_NO_WARNINGS
#include "ImageData.h"
#include "GraphManager.h"
#include <assert.h>

void CheckDevILError();

float ImageData::imageCoords[8] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };

ImageData::ImageData(FILE* file)
{
	// Read Parameters
	fread(&imageType, sizeof(ILenum), 1, file);
	fread(&imageSize, sizeof(unsigned int), 1, file);

	// Malloc Storage Space
	imageData = malloc(imageSize);
	fread(imageData, imageSize, 1, file);
}

ImageData::ImageData(char* filename)
{
	// Open an image using DevIL for inspection
	ILuint image; // The image name to return.
	ilGenImages(1, &image); // Grab a new image name.
	ilBindImage(image);
	if (ilLoadImage(filename) == IL_FALSE)
	{
		CheckDevILError();
	}
	// Do some error check
	ILint format = ilGetInteger(IL_IMAGE_FORMAT);	// This should return "Image Format" but it is returning "Data Format"
	ILint type = ilGetInteger(IL_IMAGE_TYPE);	// "Data type"
	if ((format != IL_RGBA && format!= IL_RGB)|| type != IL_UNSIGNED_BYTE)
	{
		std::cout << "[Error]Texture map is not specified format and type, in " << __FILE__ << " , line " << __LINE__ << endl; 	// __DEBUG__: Require better solution
		__debugbreak();
	}

	// __Debug__ This seems a huge bug of DevIL
	// Save Information about the image
	imageType = IL_TYPE_UNKNOWN;

	// Delete the image
	ilDeleteImage(image);

	// Save Raw Image File
	FILE* file = fopen(filename, "rb");
	fseek(file, 0L, SEEK_END);
	imageSize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	imageData = malloc(imageSize);
	fread(imageData, imageSize, 1, file);
	fclose(file);
}

ImageData::~ImageData()
{
	free(imageData);

	if (imageRenderable != NULL)
	{
		delete imageRenderable;
		glDeleteTextures(1, &imageTexture);
	}
}

bool ImageData::Save(FILE* file, unsigned int currentIndex)
{
	if (usageCount > 0)
	{
		slotIndex = currentIndex;

		// Tag a block
		unsigned char tag = IMAGE_DATA_BLOCK_TAG;
		fwrite(&tag, sizeof(unsigned char), 1, file);

		fwrite(&imageType, sizeof(ILenum),1 ,file);
		fwrite(&imageSize, sizeof(unsigned int),1, file);
		fwrite(imageData, imageSize, 1, file);

		return true;
	}
	return false;
}

void ImageData::Export(const char* name)
{
	// Generate real file name from pure file name
	static const char* suffix = ".png";
	unsigned int size = strlen(name) + strlen(suffix) + 1;
	char* filename = (char*)malloc(size);
	strcpy(filename, name);
	strcat(filename, suffix);
	filename[size -1] = 0;

	// Open an image using DevIL for inspection
	ILuint image; // The image name to return.
	ilGenImages(1, &image); // Grab a new image name.
	ilBindImage(image);
	if (ilLoadL(IL_TYPE_UNKNOWN, imageData, imageSize) == IL_FALSE)
	{
		CheckDevILError();
	}

	// Save a image
	if (ilSaveImage(filename) == IL_FALSE);	// Do a conversion since we do not know the original format, this sucks
	{
		CheckDevILError();
	}

	// Delete the image
	ilDeleteImage(image);

	// Release Resources
	free(filename);
}

RenderableImageRect* ImageData::GetImage(Interface* inParent)
{
	renderableReferenceCount++;

	// Because this is fast, so not need to bother with a thread
	if (imageRenderable != NULL)
	{
		return imageRenderable;
	}
	else
	{
		// Generate OpenGL object
		glGenTextures(1, &imageTexture);

		// Open an image using DevIL
		ILuint image; // The image name to return.
		ilGenImages(1, &image); // Grab a new image name.
		ilBindImage(image);
		if (ilLoadL(imageType, imageData, imageSize) == IL_FALSE)
		{
			CheckDevILError();
		}
		ILint format = ilGetInteger(IL_IMAGE_FORMAT);

		// Load the image into openGL
		imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
		imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, imageTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, format == IL_RGBA ? GL_RGBA8 : GL_RGB8, imageWidth, imageHeight);
		unsigned char* imageUnCompressedData = ilGetData();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, format == IL_RGBA ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageUnCompressedData);

		// Set Texture Mipmaps
		// Pending

		// Set Texture Sampling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Delete the image
		ilDeleteImage(image);

		// Debug: Pending better design to defferentiate different instances of ImageNode and Support for LandScape image node
		imageRenderable = new RenderableImageRect(inParent, (float)imageWidth/1000, (float)imageHeight/1000, imageCoords, imageTexture);
		return imageRenderable;
	}
}

void ImageData::ReturnImage()
{
	renderableReferenceCount--;

	if (renderableReferenceCount == 0)
	{
		delete imageRenderable;
		glDeleteTextures(1, &imageTexture);
		imageRenderable = NULL;
	}
}

void ImageData::RegisterUsage()
{
	usageCount++;
}

void ImageData::ReleaseUsage()
{
	assert(usageCount > 0);
	usageCount--;	// By desing this should not cause the usageCount to drop below 0, otherwise it would indicate a bug
}

int ImageData::GetSlotIndex()
{
	return slotIndex;
}

int ImageData::GetImageWidth()
{
	return imageWidth;
}

int ImageData::GetImageHeight()
{
	return imageHeight;
}