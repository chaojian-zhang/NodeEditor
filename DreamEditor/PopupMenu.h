#pragma once
#include "Interface.h"

/*Popup menu is special in that its viewport location will change dynamically depending on where it's summoned*/
class PopupMenu : public Interface
{
public:
	// Partial Interface Implementation
	virtual void OnResizeFramebuffer(int width, int height){};	// Do not care: Viewport size determined at creation time, location determined when Show()

	// PopupMenu useful Public Interactions
	void Show(unsigned int viewX, unsigned int viewY);	// Viewport Location
	void Hide();

protected:
	// Static Variables
	static int LayoutBeginPositionX;
	static int LayoutBeginPositionY;
	static unsigned int LayoutRowPadding;
	static unsigned int LayoutColumnPadding;
};