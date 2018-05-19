#include "Panel.h"
#include "GraphManager.h"
#include "Interface.h"

Panel::Panel(unsigned int inWidth, unsigned int inHeight, float imageCoords[8], GLuint panelTexture, Interface* inParent,
	unsigned int segmentWidth = 0, unsigned int segmentHeight = 0)
{
	parent = inParent;

	background = new RenderableImageRect(inParent, inWidth, inHeight, imageCoords, panelTexture);

	width = inWidth;
	height = inHeight;
}

//Panel::Panel(float imageCoords[8], GLuint panelTexture, Interface* inParent)
//{
//	parent = inParent;	// __Debug__ We might want to do this in base constructor, becasue doing it here in every child class really isn't intuitive
//
//	// Set bRelative
//	bRelative = true;
//
//	background = new RenderableImageRect(inParent, imageCoords, panelTexture, inParent);
//
//	width = 0;	// trvially Assigned
//	height = 0;
//}

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

// The order of rendering, taking transparancy into consideration is done by The Interface Using this panel object
void Panel::Render()
{
	if (bRelative)
	{
		//// Disable Pixel Perfect Rendering
		//// Hack Renderbale View Matrix
		//glm::mat4x4 temp = parent->project2DView;	// Keep a backup
		//parent->project2DView = glm::ortho<float>(0.f, 1.f, -1.f, 0.f, 0.f, 100.f) * GraphManager::View2D;
		//// Do the render
		//background->Render();
		//// Set Hacking Back
		//parent->project2DView = temp;
	}
	else
	{
		background->Render();
	}
}