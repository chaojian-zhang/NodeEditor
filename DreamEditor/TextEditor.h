#pragma once
#include "TextContainer.h"
#include "InterfaceElement.h"

// A base abstraction for SearchField and TextArea and TextField (All 2D, so Not for TextNode)
class TextEditor : public InterfaceElement
{
public:
	TextEditor(float w, float h, unsigned short* initialText, unsigned int numChars, unsigned int fSize, float inColor[3], Interface* inParent, bool singleLine, bool fixWidth, bool fixHeight, bool useReference);
	~TextEditor();

	// Base Interface Implementation
	virtual void UpdatePosition(int newx, int newy);
	virtual void Render();
	virtual void OnMouseOver(double xpos, double ypos);
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos);
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods);
	virtual void OnTextInput(unsigned short* string, unsigned int numChars);
	virtual void OnDropWindow(int count, const char** paths);
	virtual void OnMouseLeave();

	// Text Update Facility
	//void UpdateContent(unsigned short* newText, unsigned int numChars);
	void AssociateStringData(StringData* newString);

protected:
	// Children May change value
	static unsigned int XPadding;
	static unsigned int YPadding;

	// Children may query
	unsigned int GetTextHeight();

private:
	TextContainer* text;
};