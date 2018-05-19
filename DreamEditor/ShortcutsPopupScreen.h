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
	// No Interface Elements

	// Background
	static unsigned int areaWidth;
	static unsigned int areaHeight;
	static float screenTexCoords[8];
	static GLuint screenTexImage;
	Panel* backgroundImage;
};