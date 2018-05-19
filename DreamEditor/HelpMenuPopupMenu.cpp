#include "HelpMenuPopupMenu.h"
#include "GraphManager.h"
#include "Colors.h"
#include "SystemWrapper.h"
#include"Constants.h"

HelpMenuPopupMenu::HelpMenuPopupMenu(GraphManager* g)
	:graph(g)
{
	// Create Interface Elements
	shortcuts = new Button((unsigned short*)Interface_PopupMenu_Shortcuts, this, false, 0, ColorScheme::PopupButton);
	manual= new Button((unsigned short*)Interface_PopupMenu_Manual, this, false, 0, ColorScheme::PopupButton);
	about = new Button((unsigned short*)Interface_PopupMenu_About, this, false, 0, ColorScheme::PopupButton);

	// Use a layout
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(shortcuts);
	layout.AddToNewRow(manual);
	layout.AddToNewRow(about);
	layout.Dispatch();

	// Set up a background shape
	backgroundRect = new StaticShape(this, ShapeType::Shape_BorderedSquare, layout.layoutWidth, layout.layoutHeight);

	// Offset Z Height abit to avoid being overlaped by others.
	backgroundRect->OffsetPosition(0, 0, 0.2);
	manual->OffsetPosition(0, 0, 0.4);
	about->OffsetPosition(0, 0, 0.4);
	shortcuts->OffsetPosition(0, 0, 0.4);

	// Set up correct viewport size
	// Viewport X and Y are set by MainMenu upon summoning PopupMenu
	viewportW = layout.layoutWidth + LayoutBeginPositionX;
	viewportH = layout.layoutHeight - LayoutBeginPositionY;

	GraphManager::RegisterPopupMenuElements(about);
	GraphManager::RegisterPopupMenuElements(manual);
	GraphManager::RegisterPopupMenuElements(shortcuts);

	// Update Projection and View
	UpdateProjection();
}

HelpMenuPopupMenu::~HelpMenuPopupMenu()
{
	delete shortcuts;
	delete manual;
	delete about;
	delete backgroundRect;
}

// Interface Implementation
void HelpMenuPopupMenu::Render()
{
	backgroundRect->Render();
	shortcuts->Render();
	manual->Render();
	about->Render();
}

void HelpMenuPopupMenu::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == shortcuts)
	{
		graph->ShowShortCutsScreen();
	}
	if (element == about)
	{
		graph->ShowAboutScreen();
	}
	if (element == manual)
	{
		// Open the pdf using external applications
		char directoryPath[MAX_PATH];
		SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::DocumentationManualPath, directoryPath);
		SystemFunctions::OpenFileExternal(directoryPath);
	}
}