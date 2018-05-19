#include "Panel.h"
#include "GraphManager.h"
#include "Interface.h"

Panel::Panel(unsigned int inWidth, unsigned int inHeight, const float imageCoords[16 * 2], GLuint panelTexture, Interface* inParent)
{
	parent = inParent;

	background = new RenderableImageRect(inParent, inWidth, inHeight, imageCoords, panelTexture);

	width = inWidth;
	height = inHeight;
}

Panel::~Panel()
{
	delete background;
}

// Interface Implementation
void Panel::UpdatePosition(int newx, int newy)
{
	InterfaceElement::UpdatePosition(newx, newy);

	background->Translate((float)x, (float)y, -0.01);
}

void Panel::Render()
{
	background->Render();
}

void Panel::RotateU(float angle)
{
	currentRotation += angle;

	background->TextureRotationU(currentRotation);
}