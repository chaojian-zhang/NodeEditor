#pragma once
#include "InterfaceElement.h"
#include "RenderableImageRect.h"
class Interface;

class Panel : public InterfaceElement
{
public:
	Panel(unsigned int inWidth, unsigned int inHeight, const float imageCoords[16 * 2], GLuint panelTexture, Interface* inParent);
	~Panel();	// Delete Allocated Memory

	// Transformation Update
	virtual void UpdatePosition(int newx, int newy);	// Need to update all sub-renderable objects' transformations
	// Render
	virtual void Render();
	// Not used
	virtual void OnMouseOver(double xpos, double ypos){};
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos){};
	virtual void OnMouseLeave(){};
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods){};
	virtual void OnTextInput(unsigned short* text, unsigned int numChars){};
	virtual void OnDropWindow(int count, const char** paths){};

	// Extra Function
	void RotateU(float angle);
	void SetTransparency(float scale){ background->SetTransparency(scale); };

private:
	// Renderables
	RenderableImageRect* background;

	// Bookkeeping
	float currentRotation = 0;
};