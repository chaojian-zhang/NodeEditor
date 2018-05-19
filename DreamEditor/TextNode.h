#pragma once
#include "TextNodePropertyPanel.h"
#include "CanvasNode.h"
#include "Renderable3DQuoteFrame.h"
#include "RenderableRoundCornerFrame.h"
#include "RenderableRoundCornerQuad.h"
#include "Time.h"
#include "TextContainer.h"
#include "StringData.h"

/* TextNode: Load, Save, Edit Texts; Interact with TextNodePropertyPanel*/
/* Notify Property Panel when: Transformation changes*/
// STYLE CLARIFICATION: NO SCROLLING IS NEEDED UNLESS THE TEXT IS VERY LONG
class TextNode : public CanvasNode
{
public:
	// LoadingTime-Creation
	TextNode(FILE* file);	// Will Initialize Member variables and Call LoadData()
	// Runtime-Creation
	TextNode(float x, float y, float z);	// Trivial, or Not Trivial?
	~TextNode();	// Destory members

	// Interface Implementation
	virtual void Update();
	// Serialization
	virtual void SaveData(FILE* file);
	// Dynamic Streaming
	virtual void WakeUp();	// Create OpenGL Renderables
	virtual void FallAsleep();	// Destroy OpenGL Renderables, but still keep reference to the data
	virtual void Translation(float x, float y, float z);	// Override to provide specific layout

	// Interaction Interface
	virtual void OnMouseOver(double xpos, double ypos);
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos);
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods);
	virtual void OnTextInput(unsigned short* texts, unsigned int numChars);
	virtual void OnMouseLeave();
	virtual void OnDeselection();

	// Extra Functionalities
	void Export();	// Export Current Node as a Text File, rememebr to check valid characters specific to OS file naming conventions
	unsigned short* GetCreateTimeString();

private:
	// Resize Shapes
	void AnimatedResize();
	// Animation Support
	float currentWidth = 0, currentHeight = 0;
	bool bResizing = false;

private:
	// Configurations
	static float XPadding;
	static float YPadding;
	static float MinWidth;
	static float MaxWidth;
	static float MinHeight;
	static float MaxHeight;
	static unsigned int FontSize;
	static float QuoteCornerSize;
	static float FieldWidth;
	static float TextFrameColor[3];
	static float TextFrameHoverColor[3];
	static float TextBackgroundColor[3];
	static float SurfaceColor[3];
	static float SurfaceBorderColor[3];
	static float SurfaceOpacity;

private:
	// Data
	Time createTime;
	StringData* titleData;	// Although TextContainer will contain a copy(reference), but we need to keep data for personal use, since TextContainer will not always be available
	StringData* contentData;

	// State Tracker
	TextContainer* currentSelection = NULL;

	// Renderables
	Renderable3DQuoteFrame* frame;
	//RenderableRoundCornerFrame* titleField;
	RenderableRoundCornerQuad* titleField;
	RenderableRoundCornerQuad* contentField;
	TextContainer* title;
	TextContainer* content;

	// Referenced Object
	// __Pending Set up, currently we are not taking this as constructor parameter
	TextNodePropertyPanel* propertyPanel = NULL;	// Notify when tranformation information change
};