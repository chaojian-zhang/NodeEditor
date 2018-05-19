#pragma once
#include "PopupMenu.h"
#include "Button.h"
#include "GridLayout.h"
#include "StaticShape.h"

// Avoid Circular Dependancy
class MainMenu;
class GraphManager;

/*Popup menu is special in that its viewport with change dynamically depending on where it's summoned*/
class WindowMenuPopupMenu : public PopupMenu
{
public:
	WindowMenuPopupMenu(GraphManager* g);	// Set Up all elements
	~WindowMenuPopupMenu();	// Destroy all Elements

	// Render
	virtual void Render();
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);

private:
	// Interface Elements
	Button* fullscreen;
	StaticShape* backgroundRect;

	// Important referenced Object
	GraphManager* graph;
};