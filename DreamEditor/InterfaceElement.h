#pragma once
class Interface;
// VERY IMPORTANT: Due to the nature how glViewport works, the below Abs OpenGL second quadrant coordinates are actually relative to upper left of the interface viewport, not that of the window
// That is, every input is in Viewport-Relative OpenGL Second Quadrant

// Interface elements define common 2D Interface element interactions
class InterfaceElement
{
public:
	// Transformation Update
	virtual void UpdatePosition(int newx, int newy){ x = newx; y = newy; }	// Need to update all sub-renderable objects' transformations
	// Notice X and Y is integer while Z is float, that is because our camera can display in range only [-INF, 1], and we are using [0,1]; We can chanage this to use interger height in 2D elements but it would require too much change
	// since some distance offset are hardcoded in UpdatePosition and Renderables
	virtual void OffsetPosition(int dx, int dy, float dz){ x += dx; y += dy; };	// Useful in certain cases, currently we are using for popup menus/screens
	// Render Request
	virtual void Render() = 0;
	// Mouse Event handlers
	virtual void OnMouseOver(double xpos, double ypos) = 0;	// NOTICE IT IS NOT IN WINDOW COORDINATE, BUT IN Specified viewport openGl Second quadrant, as always
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos) = 0;
	// Keyboard Event handlers
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods) = 0;
	virtual void OnTextInput(unsigned short* text, unsigned int numChars) = 0;
	// Facility
	virtual void OnDropWindow(int count, const char** paths) = 0;	// Child class may decide to implement or not to

	// MouseOver event leave the interface element notification mechanism
	bool bMouseOverNotified = false;
	virtual void OnMouseLeave(){ bMouseOverNotified = false; }
	// Usage: Interface First call OnMouseOver() of the interface element, which will do related work and also turn on the boolean
	// Then next frame during the mouse FallIn check, interface element will still call OnMouseOver() if fall in, otherwise if bMouseOverNotified is set, then call OnMouseLeave()

	// Utility
	bool CheckFallIn(double x, double y);	// Given Viewport-SPecific OpenGL Second Quad cursor position, check wheteher insde the interface element
	unsigned int GetDimensionX();	// Interface Elements calculate width and height procedurally, thus creator might want to know their dimension inorder to position them
	unsigned int GetDimensionY();
	int GetLocationX();	// Can be handy
	int GetLocationY();

	Interface* parent;

protected:
	int x = 0, y = 0;	// X and Y in abs Second Quadrant openGL space; Populated by  gridlayout, set by UpdatePosition
	unsigned int width, height;	// Bookeeping,  Normally won't change; Set by Interface Element Constructors
	// Can change in case of a dynamic TextContainer
};