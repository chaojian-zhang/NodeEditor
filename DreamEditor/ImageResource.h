#pragma once
#include "ImageData.h"
#include "LinkedList.h"

// Keep track of all the imageData in the map so we know which to save when a file is saved
class ImageResource
{
public:
	~ImageResource();	// Delete all ImageData
	void Save(FILE* file);	// Find those whose reference count isn't 0, Reorder and Save

	ImageData* GetImageData(unsigned int index);

	void AddImage(ImageData* image);	// ImageLoading performed by the Node, or MapLoader when loading them; Require Abs filepath
	// unsigned int AddImage(char* filename);	// Used in run-time, return image index
	// Notice that there could be the case when user drag and drop the same file instead of using copy imageNode functions, in that case, it will expectedly cause the the image file to be stored multiple
	// times since our imageData object doesn't differentiate themselves.
	// This is by design acceptable, since we are not storing original imageNames with our imageData, even if we avoid loading the same file in run-time, we cannot make sure the loaded file doesn't collide wit
	// existing saved imageNodes

private:
	LinkedList images;
};