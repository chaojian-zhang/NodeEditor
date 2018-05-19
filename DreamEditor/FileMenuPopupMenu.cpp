#include "FileMenuPopupMenu.h"
#include "GraphManager.h"
#include "Colors.h"

FileMenuPopupMenu::FileMenuPopupMenu(GraphManager* g)
	:graph(g)
{
	// Create Interface Elements
	save = new Button((unsigned short*)Interface_PopupMenu_Save, this, false, 0, ColorScheme::PopupButton);
	exit = new Button((unsigned short*)Interface_PopupMenu_Exit, this, false, 0, ColorScheme::PopupButton);

	// Use a layout
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(save);
	layout.AddToNewRow(exit);
	layout.Dispatch();

	// Set up a background shape
	backgroundRect = new StaticShape(this, ShapeType::Shape_BorderedSquare, layout.layoutWidth, layout.layoutHeight);	// Cool: TYPE< WIDTH: HEIGHT

	// Offset Z Height abit to avoid being overlaped by other interfaces.
	backgroundRect->OffsetPosition(0, 0, 0.2);	// This this value shouldn't beyond ORTHOCAMERADISTANCE
	save->OffsetPosition(0, 0, 0.4);
	exit->OffsetPosition(0, 0, 0.4);

	// Set up correct viewport size
	viewportW = layout.layoutWidth + LayoutBeginPositionX;
	viewportH = layout.layoutHeight - LayoutBeginPositionY;

	// Setup Interaction Callbacks
	GraphManager::RegisterPopupMenuElements(save);
	GraphManager::RegisterPopupMenuElements(exit);

	// Update Projection and View
	UpdateProjection();
}

FileMenuPopupMenu::~FileMenuPopupMenu()
{
	delete backgroundRect;
	delete save;
	delete exit;
}

// Interface Implementation
// Render
void FileMenuPopupMenu::Render()
{
	backgroundRect->Render();
	save->Render();
	exit->Render();
}

void FileMenuPopupMenu::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == save)
	{
		graph->Save();
	}

	if (element == exit)
	{
		graph->Close();
	}
}