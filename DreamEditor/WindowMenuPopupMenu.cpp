#include "WindowMenuPopupMenu.h"
#include "GraphManager.h"
#include "Colors.h"

WindowMenuPopupMenu::WindowMenuPopupMenu(GraphManager* g)
	:graph(g)
{
	// Create Interface Elements
	fullscreen = new Button((unsigned short*)Interface_PopupMenu_Fullscreen, this, false, 0, ColorScheme::PopupButton);

	// Use a layout
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(fullscreen);
	layout.Dispatch();

	// Set up a background shape
	backgroundRect = new StaticShape(this, ShapeType::Shape_BorderedSquare, layout.layoutWidth, layout.layoutHeight);

	// Offset Z Height abit to avoid being overlaped by others.
	backgroundRect->OffsetPosition(0, 0, 0.2f);
	fullscreen->OffsetPosition(0, 0, 0.4f);

	// Set up correct viewport size, adhering to WindowMenu button position
	viewportW = layout.layoutWidth + LayoutBeginPositionX;
	viewportH = layout.layoutHeight - LayoutBeginPositionY;	// LayoutBeginPositionY would normally be negative

	// Setup Interaction Callbacks
	GraphManager::RegisterPopupMenuElements(fullscreen);

	// Update Projection and View
	UpdateProjection();
}

WindowMenuPopupMenu::~WindowMenuPopupMenu()
{
	delete backgroundRect;
	delete fullscreen;
}

// Interface Implementation
void WindowMenuPopupMenu::Render()
{
	backgroundRect->Render();
	fullscreen->Render();
}

void WindowMenuPopupMenu::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)	// When Interface Element is selected, call relavent functionality
{
	if (element == fullscreen && action == GLFW_PRESS)
	{
		graph->SwitchFullScreen();
	}
}