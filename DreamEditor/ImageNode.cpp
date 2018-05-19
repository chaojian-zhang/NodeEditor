#include "ImageNode.h"
#include "GraphManager.h"
#include "Colors.h"
#include "MathElegent.h"
#include "SystemWrapper.h"

// Static Variables
float ImageNode::XPadding = 0.1f;
float ImageNode::YPadding = 0.1f;
float ImageNode::MinWidth = 1;	// In meter
float ImageNode::MinHeight = 0.8f;
unsigned int ImageNode::FontSize = 16U;
float ImageNode::QuoteCornerSize = 0.1f;
float ImageNode::FieldWidth = 1.5f;
float ImageNode::TextFrameColor[3] = { 0.788f, 0.788f, 0.788f };
float ImageNode::TextFrameHoverColor[3] = { 0, 0.48, 0.769 };
float ImageNode::TextBackgroundColor[3] = { 0.23, 0.23, 0.23 };
float ImageNode::SurfaceBorderColor[3] = { 0.788f, 0.788f, 0.788f };
float ImageNode::SurfaceColor[3] = { 0.407f, 0.407f, 0.407f };
float ImageNode::SurfaceOpacity = 0.85f;
float ImageNode::ImagePixelSizeToMeterScale = 1000;

ImageNode::ImageNode(FILE* file)
{
	fread(&Tx, sizeof(float), 1, file);
	fread(&Ty, sizeof(float), 1, file);
	fread(&Tz, sizeof(float), 1, file);
	fread(&Rx, sizeof(float), 1, file);
	fread(&Ry, sizeof(float), 1, file);
	fread(&Rz, sizeof(float), 1, file);
	fread(&S, sizeof(float), 1, file);
	fread(&createTime, sizeof(Time), 1, file);

	unsigned short imageIndex;
	fread(&imageIndex, sizeof(unsigned short), 1, file);

	unsigned int descriptionNameSize;
	fread(&descriptionNameSize, sizeof(unsigned int), 1, file);
	unsigned short* tempDescriptionName = (unsigned short*)malloc(descriptionNameSize);	// Not Efficient
	fread(tempDescriptionName, descriptionNameSize, 1, file);

	unsigned int commentSize;
	fread(&commentSize, sizeof(unsigned int), 1, file);
	unsigned short* tempComment = (unsigned short*)malloc(commentSize);	// Not Efficient
	fread(tempComment, commentSize, 1, file);

	// Create 
	descNameData = new StringData(tempDescriptionName, descriptionNameSize / 2);
	commentData = new StringData(tempComment, commentSize / 2);
	imageData = GraphManager::imageResource->GetImageData(imageIndex);

	// Free resources
	free(tempComment);
	free(tempDescriptionName);
}

ImageNode::ImageNode(float x, float y, float z, char* filename)
{
	// Transformations
	Tx = x;
	Ty = y;
	Tz = z;
	Rx = Ry = Rz = 0;
	S = 1;

	// Metadata
	GraphManager::FillWithCurrentTime(&createTime);

	// Datas
	descNameData = new StringData(NULL, 0);
	commentData = new StringData(NULL, 0);
	imageData = new ImageData(filename);
	GraphManager::imageResource->AddImage(imageData);
	imageData->RegisterUsage();
}

ImageNode::~ImageNode()
{
	delete commentData;
	delete descNameData;
	imageData->ReleaseUsage();
	if (image)
	{
		imageData->ReturnImage();	// delete image;
		delete descName;
		delete comment;
		delete descField;
		delete commentField;
		delete frame;
	}
}

// Interface Implementation
void ImageNode::SaveData(FILE* file)
{
	if (bPendingDeletion)	return;

	unsigned char blockTag = IMAGE_NODE_BLOCK_TAG;
	fwrite(&blockTag, sizeof(unsigned char), 1, file);
	fwrite(&Tx, sizeof(float), 1, file);
	fwrite(&Ty, sizeof(float), 1, file);
	fwrite(&Tz, sizeof(float), 1, file);
	fwrite(&Rz, sizeof(float), 1, file);
	fwrite(&Ry, sizeof(float), 1, file);
	fwrite(&Rz, sizeof(float), 1, file);
	fwrite(&S, sizeof(float), 1, file);
	fwrite(&createTime, sizeof(Time), 1, file);
	unsigned short imageSlot = imageData->GetSlotIndex();
	fwrite(&imageSlot, sizeof(unsigned short), 1, file);
	unsigned int imageDescNameSize = descNameData->GetStringNumChars() * 2;
	fwrite(&imageDescNameSize, sizeof(unsigned int), 1, file);
	fwrite(descNameData->GetString(), imageDescNameSize, 1, file);
	unsigned int imageCommentSize = commentData->GetStringNumChars() * 2;
	fwrite(&imageCommentSize, sizeof(unsigned int), 1, file);
	fwrite(commentData->GetString(), imageCommentSize, 1, file);
}

void ImageNode::WakeUp()
{
	CanvasNode::WakeUp();

	if (bPendingDeletion)	return;

	/* Vertical Layout:
		-YPadding
		-Image
		-DescName
		-YPadding
		-Comment
		-YPadding	
	*/

	// Create Renderables
	image = imageData->GetImage(GraphManager::canvas);
	float imageWidth = (float)(imageData->GetImageWidth()) / ImagePixelSizeToMeterScale;	// Notice we must call this after we have get image;
	float imageHeight = (float)(imageData->GetImageHeight()) / ImagePixelSizeToMeterScale;
	descName = new TextContainer(GraphManager::canvas, descNameData->GetString(), descNameData->GetStringNumChars(), FontSize, Color::DefaultTextColor, 0, 0, true, true, true, false, false, descNameData);

	width = Maxf(Maxf(imageWidth, descName->GetBoundaryWidth()), MinWidth);
	comment = new TextContainer(GraphManager::canvas, commentData->GetString(), commentData->GetStringNumChars(), FontSize, Color::DefaultTextColor, width, 0, false, true, true, true, false, commentData);

	float elementsHeight = descName->GetBoundaryHeight() + YPadding * 3 + comment->GetBoundaryHeight() + imageHeight;
	height = Maxf(elementsHeight, MinHeight);
	float textAreaCornerSize = descName->GetBoundaryHeight() / 4;

	descField = new RenderableRoundCornerFrame(width, descName->GetBoundaryHeight()*1.02, TextFrameColor, textAreaCornerSize, true, false);	// Yes, Dynamic
	commentField = new RenderableRoundCornerQuad(GraphManager::canvas, width, height - YPadding * 3 - descName->GetBoundaryHeight() - imageHeight, true, TextFrameColor, TextBackgroundColor, textAreaCornerSize, true, true);

	// Notice didn't the actual rendering height of the ImageNode doesn't take the quote symbol into account, only roundCOrnered area
	width = width + XPadding * 2;
	frame = new Renderable3DQuoteFrame(width, height, true, SurfaceBorderColor, SurfaceColor, QuoteCornerSize, true, SurfaceOpacity);	// Yes, Dynamic

	// Place Elements
	Translation(Tx, Ty, Tz);

	// Add to render list, in order of rendering
	nodalRenderables.Add(frame);
	nodalRenderables.Add(image);
	nodalRenderables.Add(descField);
	nodalRenderables.Add(commentField);
	textContainers.Add(descName);
	textContainers.Add(comment);
}
void ImageNode::FallAsleep()
{
	CanvasNode::FallAsleep();

	if (bPendingDeletion)	return;

	// Delete Renderables
	imageData->ReturnImage();	// delete image;
	delete descName;
	delete descField;
	delete comment;
	delete commentField;
	delete frame;
	image = NULL;
	descName = NULL;
	descField = NULL;
	comment = NULL;
	commentField = NULL;
	frame = NULL;

	// Clear Render List
	nodalRenderables.Clear();
	textContainers.Clear();
}

void ImageNode::Translation(float x, float y, float z)
{
	CanvasNode::Translation(x, y, z);

	float imageWidth = (float)(imageData->GetImageWidth()) / ImagePixelSizeToMeterScale;
	float imageHeight = (float)(imageData->GetImageHeight()) / ImagePixelSizeToMeterScale;

	frame->Translate(x, y, z);
	image->Translate(x + XPadding, y - YPadding, z + 0.002);
	descName->Translation(x + XPadding + 0.01, y - YPadding - imageHeight, z + 0.003);	// Offset in Z depth a bit to make it not collide; Offset in X a bit so that vertical cursor position bar can be seen
	descField->Translate(x + XPadding, y - YPadding - imageHeight, z + 0.002);
	comment->Translation(x + XPadding + 0.01, y - YPadding - imageHeight - descName->GetBoundaryHeight() - YPadding, z + 0.003);// Offset in Z depth a bit to make it not collide; Offset in X a bit so that vertical cursor position bar can be seen
	commentField->Translate(x + XPadding, y - YPadding -imageHeight - descName->GetBoundaryHeight() - YPadding, z + 0.002);
}

// INteraction Interface
void ImageNode::OnMouseOver(double xpos, double ypos)
{
	CanvasNode::OnMouseOver(xpos, ypos);

	float imageHeight = (float)(imageData->GetImageHeight()) / ImagePixelSizeToMeterScale;

	// Here Check Order Matters
	if (xpos > XPadding &&
		xpos < width - XPadding &&
		ypos < -YPadding -imageHeight - descName->GetBoundaryHeight() - YPadding &&
		ypos > -(height - YPadding))
	{
		GraphManager::SetCursorIcon(CursorIcon::TextCursor);
		commentField->UpdateProperty(true, TextFrameHoverColor, TextBackgroundColor);
		descField->UpdateProperty(TextFrameColor, false);
	}
	else if (xpos > XPadding &&
		xpos < width - XPadding &&
		ypos < -YPadding - imageHeight &&
		ypos > -YPadding - imageHeight - descName->GetBoundaryHeight())
	{
		GraphManager::SetCursorIcon(CursorIcon::TextCursor);
		descField->UpdateProperty(TextFrameHoverColor, true);
		commentField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);
	}
	else
	{
		OnMouseLeave();
	}
}
void ImageNode::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	CanvasNode::OnMouseButton(button, action, mods, xpos, ypos);

	float imageHeight = (float)(imageData->GetImageHeight()) / ImagePixelSizeToMeterScale;

	if (xpos > XPadding &&
		xpos < width - XPadding &&
		ypos < -YPadding - imageHeight - descName->GetBoundaryHeight() - YPadding &&
		ypos > -(height - YPadding))
	{
		currentSelection = comment;
		currentSelection->OnMouseButton(button, action, mods, xpos - XPadding, ypos - (-YPadding - imageHeight - descName->GetBoundaryHeight() - YPadding));
	}
	else if (xpos > XPadding &&
		xpos < width - XPadding &&
		ypos < -YPadding - imageHeight &&
		ypos > -YPadding - imageHeight - descName->GetBoundaryHeight())
	{
		currentSelection = descName;
		currentSelection->OnMouseButton(button, action, mods, xpos - XPadding, ypos - (-YPadding - imageHeight));
	}
	else
	{
		currentSelection = NULL;
	}

	GraphManager::ShowImageNodeProperty(this);
}
void ImageNode::OnKeyboardButton(int key, int scancode, int action, int mods)
{
	// Summon Active TextContainers to do the work
	if (currentSelection)
	{
		currentSelection->OnKeyboardButton(key, scancode, action, mods);
	}
	else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		bPendingDeletion = true;
		FallAsleep();
	}
}
void ImageNode::OnTextInput(unsigned short* text, unsigned int numChars)
{
	if (bPendingDeletion)	return;

	// Add Texts to the Active TextContainer
	if (currentSelection)
	{
		currentSelection->OnTextInput(text, numChars);
	}
}

void ImageNode::OnMouseLeave()
{
	CanvasNode::OnMouseLeave();

	if (bPendingDeletion)	return;

	descField->UpdateProperty(TextFrameColor, false);	// Default not render
	commentField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);
	GraphManager::SetCursorIcon(CursorIcon::NormalCursor);
}

void ImageNode::OnDeselection()
{
	if (currentSelection)
	{
		currentSelection->SpecifyCursorRender(false);
	}
	currentSelection = NULL;
}

// Extra Functionalities
void ImageNode::Export()
{
	if (bPendingDeletion)	return;

	char pathBuffer[MAX_PATH];
	const char* filename = SystemFunctions::CreateFilename(descNameData->GetString(), descNameData->GetStringNumChars(), pathBuffer);

	// Save Image
	imageData->Export(filename);	// ImageData will append a suffix to the file

	// Save Comment
	commentData->Export(filename, &createTime);	// StringData will append a suffix to the file
}