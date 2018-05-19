#pragma once
#include "TextEditor.h"
#include "TextContainer.h"
#include "RenderableImageIcon.h"
#include "RenderableRoundSidesQuad.h"

// A Text Field with a search icon, and a cornered box frame
class SearchField : public TextEditor
{
public:
	SearchField(unsigned short* initialString, unsigned int numChars, Interface* inParent);
	~SearchField();	// Destroy Memory

	// Interface Modification
	virtual void UpdatePosition(int newx, int newy);
	virtual void Render();
	virtual void OnMouseOver(double xpos, double ypos);
	virtual void OnMouseLeave();

	// Interface Extension
	virtual void OnTextInput(unsigned short* string, unsigned int numChars);

private:
	static int CornerSize;
	static int IconSize;
	static float FrameColor[3];
	static float FrameHoverColor[3];
	static float BackGroundColor[3];
	static float SearchIconCoords[8];

private:
	// Renderables
	RenderableImageIcon* icon;
	RenderableRoundSidesQuad* frame;
};

/* Unfinished:
	Search Field Interaction with Canvas to actually do some search
*/

/* Pending:
	1. Search Field Expansion Animaion when mouse clicked on it to begin enter
*/