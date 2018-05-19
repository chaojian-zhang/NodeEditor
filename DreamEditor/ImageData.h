#pragma once
#include <IL\il.h>
#include "RenderableImageRect.h"
class Interface;

// Caution Library Function Thread-Safety
// But this object in our current implementation isn't using a seperate thread
class ImageData
{
public:
	// ImageData are constructed: from meory, or from file
	ImageData(char* filename);	// Runtime DragnDrop
	ImageData(FILE* file);	// Load Time
	~ImageData();	// We are still responsible for releasing the data when destroy

	bool Save(FILE* file, unsigned int currentIndex);	// This function is called by ImageResource object
	// Input current image slot index, output whether or not saved
	void Export(const char* name);	// We will append a suffix to the pure name

	// OpenGL Object Creation
	RenderableImageRect* GetImage(Interface* inParent);
	void ReturnImage();

	// Data Management
	void RegisterUsage();	// Called by ImageResource, ImageNode doesn't need to call this one
	void ReleaseUsage();	// ImageNode need to call this to make sure notify image data decrese reference count
	int GetSlotIndex();
	int GetImageWidth();	// Return in Pixels
	int GetImageHeight();

private:
	// Renderable Management
	short renderableReferenceCount = 0;	// Count to those who are using the renderable; a value of -1 indicate somewhere we have an error

	// Data Saving Management
	unsigned short usageCount = 0;	// If anyone use(reference) this, it should be bigger than  or euqla to 1; when decrese to 0. will not be saved
	int slotIndex = -1;	// -1 indicate haven't been saved, this may occur when we have wrong order of saving world data(i.e. ImageNode before imageResource)

	// Datas
	// ILint format;	// Didn't save this parameter because it will need to be initialized both when creating from file or loading from document, so better just fetch it whenever needed
	int imageWidth = 0, imageHeight = 0;
	ILenum imageType;	// Contain ILenum type for the image file
	unsigned int imageSize;	// In bytes: First retrived when reading from the file
	void* imageData;	// This should be raw data of the disk file, nor DevIL file

	// Renderable
	RenderableImageRect* imageRenderable = NULL; // Only Create When needed
	GLuint imageTexture;
	static float imageCoords[8];
};

// Usage
// Loading: From Saved Dream Map file or Drag-n-Drop
// For Image Nodes: ImageNode either get this from saved file, or Drag-n-Drop, in either case, it only usees it. But when an image node is deleted, it should Unregister.
//	 Also, when an imagenode is going to sleep, it should call ReturnImage()