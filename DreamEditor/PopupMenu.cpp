#include "PopupMenu.h"
#include "GraphManager.h"

int PopupMenu::LayoutBeginPositionX = 2U;
int PopupMenu::LayoutBeginPositionY = -2;
unsigned int PopupMenu::LayoutRowPadding = 6U;
unsigned int PopupMenu::LayoutColumnPadding = 2U;

void PopupMenu::Show(unsigned int viewX, unsigned int viewY)
{
	viewportX = viewX;
	viewportY = viewY;
	GraphManager::currentPopup = this;
}

void PopupMenu::Hide()
{
	GraphManager::currentPopup = NULL;
}