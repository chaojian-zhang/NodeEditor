#pragma once
#include "InterfaceElement.h"
#include "Renderable.h"
enum ShapeType
{
	Shape_BorderedSquare,
	Shape_Star	// ....
};

/* Static Shape provides creation of different static shapes, which can be used eiher as background or decoration,
since Static Shapes are not counted for collision detection or an other interaction*/
// All shapes renderables, of course, are created procedurally
class StaticShape :public InterfaceElement
{
public:
	StaticShape(Interface* inParent, ShapeType type, unsigned int w = 0, unsigned int h = 0);	// Windt hand Height represent the shape's realm
	~StaticShape();

	// Interface Implementation
	virtual void Render();
	virtual void UpdatePosition(int newx, int newy);
	virtual void OffsetPosition(int dx, int dy, float dz);
	// Not Used
	virtual void OnMouseOver(double xpos, double ypos){}
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos){}
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods){}
	virtual void OnTextInput(unsigned short* text, unsigned int numChars){}
	virtual void OnDropWindow(int count, const char** paths){}

private:
	Renderable* shape;	// Generic pointer to an renderable
};