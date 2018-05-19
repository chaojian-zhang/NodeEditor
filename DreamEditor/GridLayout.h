#pragma once
#include "Interface.h"
#include "InterfaceElement.h"
#include "LinkedList.h"

enum GridAlignment
{
	Align_Center,
	Align_Up,
	Align_Down,
	Align_Left,
	Align_Right,
	Align_UpLeft,
	Align_UpRight,
	Align_LowerLeft,
	Align_LowerRight
};

// Grid Structure for bookkeeping
struct Grid
{
	unsigned int gridHeight = 0;	// The max height of a grid element
	unsigned int gridWidth = 0;		// The width of elements added together, normally smaller than the row width for the grid
	GridAlignment alignment;
	LinkedList elements;	// Normally there is only one, can be more if we want to do so
};

// By convention, grids are arranged row follow rows from up to down
struct Row
{
	unsigned int maxHeight = 0;	// A single row will share a single height
	unsigned int rowWidth = 0;	// The width of all elements in the row
	LinkedList grids;
};

/* Gridlayout provide advanced aligned layout arrangement in order to properly locate each interface element within a interface
	- Gridlayout is designed only for 2D purposes, since in Canvas nodes there are expected not to have complicated layout specification
	- A GridLayout has fluid size width and height
*/
class AutoExpandGridLayout
{
public:
	// In openGL Second quadrant(Upper left origin)
	AutoExpandGridLayout(int originX, int originY, unsigned int inRowPadding, unsigned int inColumnPdding);

	// Different Elements occupy the same slot; Will override existing grid alignment
	void AddToGrid(InterfaceElement* element, GridAlignment alignment = Align_Left);
	// Start a new row
	void AddToNewRow(InterfaceElement* element, GridAlignment alignment = Align_Left);
	// Continue a new column
	void AddToRow(InterfaceElement* element, GridAlignment alignment = Align_Left);

	// Calculate location according to space occupation of elements and assign them to the elements
	void Dispatch();

	int layoutWidth, layoutHeight;	// Provide final calculated layout size
	// Used By Popup Menu Interfaces, whose interface creation(background shape) depends on the calculated layout size

private:
	LinkedList rows;
	// LinkedList maxWidths;	// The number is the same as the number of rows with different widht; The rows with the same number of Girds will use the same width
	// Due to technical difficulty we are not considering veritical alignment for now, just horizontal alignment

	int beginOffsetX, beginOffsetY; // x, y in openGL second quadrant relative to viewport
	unsigned int rowPadding, columnPadding;
};	