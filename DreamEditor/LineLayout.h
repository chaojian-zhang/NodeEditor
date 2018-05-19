#pragma once
#include "Interface.h"
#include "InterfaceElement.h"
// DEPRECATED--------------------------------------------------
/* A linear layout is responsible for transforming zero-positioned interface elements to the places they need to be*/
// Interface elements are created within Interface obejct but maintained inside layout object, thus destoryed when the layout is destroyed
// Linelayout doens't received events, but contain MovePosition() for positioning
class LineLayout
{
public:
	LineLayout(int inX, int inY, Interface* parent);
	~LineLayout();	// Responsible for deleting interface elements
	void AddElement(InterfaceElement* element);	// Beside add to Layout list, also responsible for updating element posiions

	void MovePosition(int dx, int dy);	// A line layout might be panned, it will be responsible for updating all containted interface elements

	// Get Elements Routines
	void GetElement(int index);
	void GetElementByPosition(double xpos, double ypos);	// Abs OGL Sec Quad Coord
private:
	// Also managed/contained within an interface area, its coordinates are specified in opeNGL Second Quadrant Absolute
	int x, y;
	int heightSpec;	// A layout with have a Specified height, but its width is calculated based on content(elements within)
	// Height is used such that, giving the height of an element, it will be positioned in the center;
	// THe height of the elements are not controlled, but specified at creation by Interface object itself
private:
	int widthOccupy;	// Make it a member variable here only for bookkeeping
	int numElements;
	InterfaceElement* elements;
	Interface* parentInterface;	// Useful for extracting Viewport dimension data
};