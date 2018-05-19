#ifndef INTERFACECOMPONENTS_H
#define INTERFACECOMPONENTS_H
#include "Widgets.h"
// Used by main menu bar
class PopupMenu
{

};

// Used by Sidepanel and Canvas for navigation
class Camera
{

};

// The three interfaces can have different viewports in some graphs, or they may just write on the same viewport, i.e. Viewports can overlap each other
struct Viewport
{
	unsigned short x;	// Specified from UP to LR
	unsigned short y;	// Specified from UP to LR
	unsigned short width;
	unsigned short height;
};

// Menubar and Sidepanel to use are all just interface drawing areas, since they are implemented using the same components
class InterfaceArea
{
protected:
	Widget** widgets;
};

class Menubar: public InterfaceArea
{
private:
	// Menubar Exclusive objects
	PopupMenu** popupMenus;
};

class SidePanel: public InterfaceArea
{
private:
	// Side Panel Special Objects
	Camera* sidePanelCamera;
};

// Canvas is conceptualy a seperate 3D space, but some 3D interface components like labels might also be useful
class Canvas: public InterfaceArea
{
private:
	// Canvas Objects
	Camera* canvasCamera;
};
#endif