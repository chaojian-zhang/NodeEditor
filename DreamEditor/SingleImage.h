//#pragma once
//#include "InterfaceElement.h"
//#include "RenderableImageRect.h"
//class Interface;
//
//class SingleImage : public InterfaceElement
//{
//public:
//	SingleImage(float width, float height, GLuint texture, Interface* inParent);
//	~SingleImage();	// Delete Allocated Memory
//
//	// Transformation Update
//	virtual void UpdatePosition(int newx, int newy);
//	virtual void OffsetPosition(int dx, int dy, float dz);
//	// Render
//	virtual void Render();
//	virtual void OnMouseOver(double xpos, double ypos){}
//	virtual void OnMouseSingleImage(int SingleImage, int action, int mods, double xpos, double ypos){}
//	virtual void OnMouseLeave(){}
//	// Not used
//	virtual void OnKeyboardSingleImage(int key, int scancode, int action, int mods){};
//	virtual void OnTextInput(unsigned short* text, unsigned int numChars){};
//	virtual void OnDropWindow(int count, const char** paths){};
//
//protected:
//	// Renderables
//	RenderableImageRect* image = NULL;
//	static float imageCoords[8];
//};