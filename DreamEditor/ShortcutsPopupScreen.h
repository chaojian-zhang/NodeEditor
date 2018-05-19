#pragma once
#include "PopupMenu.h"
#include "GridLayout.h"
#include "Panel.h"
#include "TextLabel.h"

/*Popup menu is special in that its viewport with change dynamically depending on where it's summoned*/
class ShortcutsPopupScreen : public PopupMenu
{
public:
	ShortcutsPopupScreen();
	~ShortcutsPopupScreen();	// Destroy all Elements

	// Render
	void Render();

private:
	// Interface Elements
	// All are informative elements
	//TextLabel* line1;
	//TextLabel* line2;
	//TextLabel* line3;

	// Background
	static unsigned int areaWidth;
	static unsigned int areaHeight;
	static float screenTexCoords[8];
	static GLuint screenTexImage;
	Panel* backgroundImage;

private:
	// Use a linked list to help managing
	// LinkedList labels;
};