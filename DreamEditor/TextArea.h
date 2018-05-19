#pragma once
#include "TextEditor.h"
#include "TextContainer.h"
#include "RenderableRoundCornerQuad.h"

// NOtice InterfaceElement is 2D, so TextArea is 2D
class TextArea : public TextEditor
{
public:
	TextArea(float w, float h, unsigned short* initialText, unsigned int numChars, Interface* inParent, bool useReference);
	~TextArea();

	// Interface Implmentation
	virtual void UpdatePosition(int newx, int newy);
	virtual void Render();
	virtual void OnMouseOver(double xpos, double ypos);
	virtual void OnMouseLeave();

private:
	// Configurations
	static int CornerSize;
	static float FrameColor[3];
	static float FrameHoverColor[3];
	static float BackGroundColor[3];

private:
	// Renderable Objects
	RenderableRoundCornerQuad* background;
};