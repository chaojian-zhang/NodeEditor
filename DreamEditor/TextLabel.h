#pragma once
#include "InterfaceElement.h"
#include "RenderableString.h"

class TextLabel : public InterfaceElement
{
public:
	TextLabel(unsigned short* initialText, bool dynamic, Interface* inParent);
	TextLabel(unsigned short* initialText, bool dynamic, float inColor[3], Interface* inParent);
	~TextLabel();

	// Interface Implmentation
	virtual void UpdatePosition(int newx, int newy);
	virtual void OffsetPosition(int dx, int dy, float dz);
	virtual void Render();
	virtual void OnMouseOver(double xpos, double ypos){};
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos){};
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods){};
	virtual void OnTextInput(unsigned short* text, unsigned int numChars){};
	virtual void OnDropWindow(int count, const char** paths){};

	// Update Facility
	void UpdateContent(unsigned short* newText, float inColor[3] = NULL);
	void UpdateTransparancy(float value);	// Animations can use this

private:
	// Renderable Objects
	RenderableString* text;
};