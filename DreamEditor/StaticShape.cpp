#include "StaticShape.h"
#include "RenderableRoundCornerQuad.h"
#include "Colors.h"

StaticShape::StaticShape(Interface* inParent, ShapeType type, unsigned int w, unsigned int h)
{
	parent = inParent;

	// Set Up Parameters
	width = w, height = h;

	// Create the Shape
	switch (type)
	{
	case Shape_BorderedSquare:
		shape = new RenderableRoundCornerQuad(parent, width, height, true, Color::DefaultRoundCornerQuadBoarderColor, Color::DefaultRoundCornerQuadSurfaceColor, 0, false, false);
		break;
	case Shape_Star:
		shape = NULL;
		break;
	default:
		break;
	}
}

StaticShape::~StaticShape()
{
	delete shape;
}

// Interface Implementation
void StaticShape::Render()
{
	shape->Render();
}

void StaticShape::UpdatePosition(int newx, int newy)
{
	InterfaceElement::UpdatePosition(newx, newy);
	
	shape->Translation(x, y, 0);
}

void StaticShape::OffsetPosition(int dx, int dy, float dz)
{
	InterfaceElement::OffsetPosition((float)dx, (float)dy, dz);

	shape->Translate((float)dx, (float)dy, dz);
}