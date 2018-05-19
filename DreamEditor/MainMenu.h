#pragma once
#include "Interface.h"
#include "Button.h"
#include "SearchField.h"
#include "Panel.h"
#include "FileMenuPopupMenu.h"
#include "WindowMenuPopupMenu.h"
#include "HelpMenuPopupMenu.h"

// Debug_Temp
#include "TextBox.h"

class MainMenu : public Interface
{
public:
	MainMenu();	// Set Up Interface Elements: Assign String and Size Suggestions(used to calculate bound box)
	~MainMenu();	// Destroy Elements
	void RegisterPopups(FileMenuPopupMenu* f, WindowMenuPopupMenu* w , HelpMenuPopupMenu* h);

	// Render Menus and Search Field according to their state
	virtual void Render();
	// Mouse: Handle Mouse Over Button, and Mouse Selection
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);
	// Framebuffer: Upon resize, our viewport will be changed horizontally, but that doesn't matter
	virtual void OnResizeFramebuffer(int width, int height);	// Take Care of Viewport Change

	// PopupMenu Display Processing
	void OnElementMouseOver(InterfaceElement* element);

	// Layers Update
	void LayerRotate(float angle);

public:
	// Static Variable as Configuration Setting
	static unsigned int MainMenuHeight;
	static unsigned int ButtonRowPadding;
	static unsigned int ButtonColumnPadding;
	static float MainMenuTexCoords_Bottom[8];
	static float MainMenuTexCoords_Middle[8];
	static float MainMenuTexCoords_Top[8];

private:
	// Bookkeeping
	InterfaceElement* currentSelection; // Do a dynamic cast to see whether our selection is SearchField, thus processing text input

	// Interface Elements
	Button* file;
	Button* window;
	Button* help;
	// SearchField* search;
	Panel* background;
	Panel* midground;
	Panel* foreground;
	// Might also add a frame that doesn't rotate with camera
	// Panel* viewFrame;
	// StandAloneIcon* appIcon; // Blender-like Application Information presentation, icon; StandaloneIcon is not implemented because for the most cases icon imageQuad renderable comes with specific interface elements, i.e. not need to provide with a stand along one
	// TextContainer* appInfo;	// Blender-like Application Information presentation, e.g. Application Name, current version
	// This can also be included in About popup screen

	// Important Referenced Objects
	FileMenuPopupMenu* filePopup;
	WindowMenuPopupMenu* windowPopup;
	HelpMenuPopupMenu* helpPopup;
};