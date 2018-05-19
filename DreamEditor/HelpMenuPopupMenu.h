#pragma once
#include "PopupMenu.h"
#include "StaticShape.h"
#include "ShortcutsPopupScreen.h"
#include "AboutPopupScreen.h"
#include "Button.h"
#include "GridLayout.h"
class GraphManager;

/*Popup menu is special in that its viewport with change dynamically depending on where it's summoned*/
class HelpMenuPopupMenu : public PopupMenu
{
public:
	HelpMenuPopupMenu(GraphManager* g);	// Set Up all element
	~HelpMenuPopupMenu();	// Destroy all Elements

	// Render
	virtual void Render();
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);

private:
	// Interface Elements
	Button* shortcuts;
	Button* manual;
	Button* about;
	StaticShape* backgroundRect;

	// Important Reference Objects
	GraphManager* graph;
};