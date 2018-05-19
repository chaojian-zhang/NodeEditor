#include "InterfaceElement.h"
bool InterfaceElement::CheckFallIn(double px, double py)	// Abs OpenGL Second Quadrant Relative to interface viewport
{
	// Debug
	//printf("[Debug]IE %i->%i, %i->%i Check Fall in: %f, %f\n", x, width + x, y, y-height, px, py);

	if (px >= x && px <= x + width && py <= y && py >= y - (int)height)	// Notice width and height are positive numbers
	{
		return true;
	}

	return false;
}

unsigned int InterfaceElement::GetDimensionX()
{
	return width;
}

unsigned int InterfaceElement::GetDimensionY()
{
	return height;
}

int InterfaceElement::GetLocationX()
{
	return x;
}

int InterfaceElement::GetLocationY()
{
	return y;
}