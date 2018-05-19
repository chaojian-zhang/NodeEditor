#include "ImageResource.h"

ImageResource::~ImageResource()
{
	ImageData* data = (ImageData*)images.First();

	while (data != NULL)
	{
		delete data;
		data = (ImageData*)images.Next();
	}
}

void ImageResource::Save(FILE* file)
{
	ImageData* data = (ImageData*)images.First();
	
	unsigned int imageSlot = 0;
	while (data != NULL)
	{
		if (data->Save(file, imageSlot))
		{
			imageSlot++;
		}
		data = (ImageData*)images.Next();
	}
}

ImageData* ImageResource::GetImageData(unsigned int index)
{
	ImageData* image = (ImageData*)(images.Select(index));
	image->RegisterUsage();
	return image;
}

void ImageResource::AddImage(ImageData* image)
{
	images.Add(image);
}

//unsigned int ImageResource::AddImage(char* filename)
//{
//	images.Add(new ImageData(filename));
//	return images.Length();	// Why Not -1?
//}