#pragma once
#include "ImageData.h"
#include "Time.h"
#include "ImageNodePropertyPanel.h"
#include "CanvasNode.h"
#include "RenderableImageRect.h"
#include "RenderableRoundCornerFrame.h"
#include "Renderable3DQuoteFrame.h"
#include "TextContainer.h"
#include "StringData.h"

/* Interaction with Property Panel: */
class ImageNode : public CanvasNode
{
public:
	// Constructor
	ImageNode(FILE* file);	// Load-time
	ImageNode(float x, float y, float z, char* filename);	// Run-time; Abs path
	~ImageNode();

	// Interface Implementation
	// Serialization
	virtual void SaveData(FILE* f);
	// Dynamic Streaming
	virtual void WakeUp();	// Create OpenGL Renderables
	virtual void FallAsleep();	// Destroy OpenGL Renderables, but still keep reference to the data
	virtual void Translation(float x, float y, float z);	// Override to provide specific layout
	//virtual void Translate(float dx, float dy, float dz);	// Override to provide specific layout // Shoudnn't be needed

	// Interaction Interface
	virtual void OnMouseOver(double xpos, double ypos);
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos);
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods);
	virtual void OnTextInput(unsigned short* text, unsigned int numChars);
	virtual void OnMouseLeave();
	virtual void OnDeselection();

	// Extra Functionalities
	void Export();
	unsigned short* GetCreateTimeString(){ return createTime.GetString(); }

private:
	// Configurations
	static float XPadding;
	static float YPadding;
	static float MinWidth;
	static float MinHeight;
	static unsigned int FontSize;
	static float QuoteCornerSize;
	static float FieldWidth;
	static float TextFrameColor[3];
	static float TextFrameHoverColor[3];
	static float TextBackgroundColor[3];
	static float SurfaceColor[3];
	static float SurfaceBorderColor[3];
	static float ImagePixelSizeToMeterScale;
	static float SurfaceOpacity;

private:
	// Object Data
	Time createTime;
	StringData* commentData;
	StringData* descNameData;
	ImageData* imageData;

	// State Tracker
	TextContainer* currentSelection = NULL;

private:
	// Renderables: Created When Wake Up
	Renderable3DQuoteFrame* frame = NULL;
	RenderableImageRect* image = NULL;	// Notice this doesn't belong to us, but to ImageData
	RenderableRoundCornerFrame* descField;
	RenderableRoundCornerQuad* commentField;
	TextContainer* descName = NULL;	// DescName doesn't have a field frame decoration
	TextContainer* comment = NULL;

private:
	// Referenced Object
	// __Pending Set up, currently we are not taking this as constructor parameter
	ImageNodePropertyPanel* propertyPanel = NULL;	// Notify when tranformation information change
};