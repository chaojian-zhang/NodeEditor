#include "MainMenu.h"
#include "GridLayout.h"
#include "Constants.h"
#include "GraphManager.h"
#include "MaterialTextures.h"

// Static Variable
unsigned int MainMenu::MainMenuHeight = 26U; 	// Why 26? This is By design
unsigned int MainMenu::ButtonRowPadding = 16U; 	// Each Button Should Have a 16 pixel gap
unsigned int MainMenu::ButtonColumnPadding = 4U; 	// Not Used
float MainMenu::MainMenuTexCoords_Bottom[8] = { 	// Clockwise
	0.0f, 0.0f, 
	1.0f, 0.0f, 
	1.0f, 63.0f / 2048.f, 
	0.0f, 63.0f / 2048.f };
float MainMenu::MainMenuTexCoords_Middle[8] = { 	
	0.0f, 64.0f / 2048.f,
	1.0f, 64.0f / 2048.f,
	1.0f, 127.0f / 2048.f,
	0.0f, 127.0f / 2048.f };
float MainMenu::MainMenuTexCoords_Top[8] = { 
	0.0f, 128.0f / 2048.f,
	1.0f, 128.0f / 2048.f,
	1.0f, 191.0f / 2048.f,
	0.0f, 191.0f / 2048.f };

MainMenu::MainMenu()
{
	// Create MainMenu Texture
	char pathBuffer[MAX_PATH];
	GLuint MainMenuTexImage = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath, pathBuffer));

	// Create Interface Obejcts
	file = new Button((unsigned short*)Interface_Filemenu, this);
	window = new Button((unsigned short*)Interface_Windowmenu, this);
	help = new Button((unsigned short*)Interface_Helpmenu, this);
	//search = new SearchField(NULL, 0, this);
	background = new Panel(GraphManager::windowWidth, MainMenuHeight, MainMenuTexCoords_Bottom, MainMenuTexImage, this);
	midground = new Panel(GraphManager::windowWidth, MainMenuHeight, MainMenuTexCoords_Middle, MainMenuTexImage, this);
	foreground = new Panel(GraphManager::windowWidth, MainMenuHeight, MainMenuTexCoords_Top, MainMenuTexImage, this);
	// We have already disabled depth test in GraphManager so no need to do extra offset here, otherwise we need to manually offset a bit to avoid foreground and midground being clipped.

	// Layout Things
	AutoExpandGridLayout layout(4, -4, ButtonRowPadding, ButtonColumnPadding);	// Give it some distance from the viewport origin
	layout.AddToRow(file);
	layout.AddToRow(window);
	layout.AddToRow(help);
	//layout.AddToRow(search);

	layout.Dispatch();

	// Interface Properties
	bHidden = false;
	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	// Register Input Interaction with Graph Manager
	GraphManager::RegisterMainMenuElements(file);
	GraphManager::RegisterMainMenuElements(window);
	GraphManager::RegisterMainMenuElements(help);
	//GraphManager::RegisterMainMenuElements(search);
}

MainMenu::~MainMenu()
{
	delete file;
	delete window;
	delete help;
	//delete search;
	delete background;
	delete midground;
	delete foreground;
}

void MainMenu::RegisterPopups(FileMenuPopupMenu* f, WindowMenuPopupMenu* w, HelpMenuPopupMenu* h)
{
	filePopup = f;
	windowPopup = w;
	helpPopup = h;
}

// Interface Implementations
void MainMenu::Render()
{
	if (!bHidden)	// Mainmenu can be hidden by Fullscren button or fullscreen shortcut Alt+F10
	{
		// Render background first
		background->Render();
		midground->Render();
		foreground->Render();
		file->Render();
		window->Render();
		help->Render();

		//search->Render();
	}
}

void MainMenu::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == file)
	{
		// Summon Popup Menu
		unsigned int viewX = viewportX + file->GetLocationX();
		unsigned int viewY = GraphManager::windowHeight + file->GetLocationY()/*This is a negative number since we are in different spaces*/ - file->GetDimensionY() - filePopup->viewportH;
		filePopup->Show(viewX, viewY);
	}
	else if (element == window)
	{
		// Summon Popup Menu
		unsigned int viewX = viewportX + window->GetLocationX();
		unsigned int viewY = GraphManager::windowHeight + window->GetLocationY() - window->GetDimensionY() - windowPopup->viewportH;
		windowPopup->Show(viewX, viewY);
	}

	else if (element == help)
	{
		// Summon Popup Menu
		unsigned int viewX = viewportX + help->GetLocationX();
		unsigned int viewY = GraphManager::windowHeight + help->GetLocationY() - help->GetDimensionY() - helpPopup->viewportH;
		helpPopup->Show(viewX, viewY);
	}
	//else if (element == search)  // Search Field Activation(Search Field Animation)
	//{

	//}
	else
	{
		// Do nothing
	}
}

void MainMenu::OnResizeFramebuffer(int width, int height)
{
	// Notice these are relative to Lower-Left corner of the window
	viewportX = 0;
	viewportY = height - MainMenuHeight;
	viewportW = width;
	viewportH = MainMenuHeight;
	
	// Update Projection and View
	UpdateProjection();
}

void MainMenu::OnElementMouseOver(InterfaceElement* element)
{
	// If this is a button, and we are currently displaying a popupMenu, then we want to display another popupmenu

	if (element == file)
	{
		// Summon Popup Menu
		unsigned int viewX = viewportX + file->GetLocationX();
		unsigned int viewY = GraphManager::windowHeight + file->GetLocationY() - file->GetDimensionY() - filePopup->viewportH;
		filePopup->Show(viewX, viewY);
	}
	else if (element == window)
	{
		// Summon Popup Menu
		unsigned int viewX = viewportX + window->GetLocationX();
		unsigned int viewY = GraphManager::windowHeight + window->GetLocationY() - window->GetDimensionY() - windowPopup->viewportH;
		windowPopup->Show(viewX, viewY);
	}

	else if (element == help)
	{
		// Summon Popup Menu
		unsigned int viewX = viewportX + help->GetLocationX();
		unsigned int viewY = GraphManager::windowHeight + help->GetLocationY() - help->GetDimensionY() - helpPopup->viewportH;
		helpPopup->Show(viewX, viewY);
	}
	/*else if (element == search)
	{
		GraphManager::currentPopup = NULL;
	}*/
	else
	{
		GraphManager::currentPopup = NULL;
	}
}

void MainMenu::LayerRotate(float angle)
{
	// Style 1
	// Rotate the different grounds at different direction and speed(well that was distracting so the below lines use the same direction instead)
	//background->RotateU(angle/(8*3.14));	// Sqaure fall off
	//midground->RotateU(angle/(4*3.14));
	//foreground->RotateU(angle/(2*3.14));	// Foreground rotate at the same speed as camera, in the same direction; But same speed is too fast, so half the speed

	// Style 2
	// Foreground fixed, mid and back rotate at same direction, this makes more sense and is less distracting
	background->RotateU(angle/(8*3.14));
	midground->RotateU(angle/(4*3.14));
	// foreground->RotateU(0);	
}

// __Debug__ Currenlty no fill the width
// background->UpdateProjection(background->GetDimensionX(), background->GetDimensionY());	// Keep faith to himself