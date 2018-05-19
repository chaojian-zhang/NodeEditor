#pragma once
#include "PopupMenu.h"
#include "Button.h"
#include "StaticShape.h"
#include "GridLayout.h"

// Avoid Circular Dependancy
class GraphManager;

/*Popup menu is special in that its viewport with change dynamically depending on where it's summoned*/
class FileMenuPopupMenu : public PopupMenu
{
public:
	FileMenuPopupMenu(GraphManager* g);	// Set Up all elements
	~FileMenuPopupMenu();	// Destroy all Elements

	// Interface
	virtual void Render();
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);

private:
	// Interface Elements
	Button* save;
	//	Button* exportDocument;	// I assure myself will never need this
	Button* exit;
	StaticShape* backgroundRect;

	// Important Reference Functions
	GraphManager* graph;
};