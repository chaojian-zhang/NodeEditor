#pragma once
#include "PopupMenu.h"
#include "Panel.h"
#include "GridLayout.h"
#include "TextLabel.h"
#include "LinkedList.h"

/*Popup menu is special in that its viewport with change dynamically depending on where it's summoned*/
class AboutPopupScreen : public PopupMenu
{
public:
	AboutPopupScreen();	// Set Up all element: the actual extent of the Popup menu will be bigger than display are, so that we can have a tolerance mouse cursor distance: This is now done by Graphmanager
	~AboutPopupScreen();	// Destroy all Elements

	// Render
	void Render();

private:
	// Interface Elements
	// All are informative elements
	TextLabel* original;
	TextLabel* contact;

	// Background
	static unsigned int areaWidth;
	static unsigned int areaHeight;
	static float screenTexCoords[8];
	static GLuint screenTexImage;
	Panel* backgroundImage;

private:
	// Use a linked list to help managing
	LinkedList labels;
};