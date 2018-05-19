#include "ShortcutsPopupScreen.h"
#include "Colors.h"
#include "MaterialTextures.h"
#include "SystemWrapper.h"
#include "Constants.h"

#define AboutPop_HorizontalPadding 8 // Randomly selected padding for this screen, accidentally to be identical to that of the AboutScreen
#define AboutPop_VerticalPadding 4

unsigned int ShortcutsPopupScreen::areaWidth = 850U;
unsigned int ShortcutsPopupScreen::areaHeight = 500U;
float ShortcutsPopupScreen::screenTexCoords[8] = {
	0.0f / 2048.0f, 1024.0f / 2048.0f,
	850.0f / 2048.0f, 1024.0f / 2048.0f,
	850.0f / 2048.0f, 1524.0f / 2048.0f,
	0.0f / 2048.0f, 1524.0f / 2048.0f };
GLuint ShortcutsPopupScreen::screenTexImage = NULL;

ShortcutsPopupScreen::ShortcutsPopupScreen()
{
	// Set up correct viewport size
	viewportW = areaWidth;
	viewportH = areaHeight;

	// Set up a background image
	char pathBuffer[MAX_PATH];
	screenTexImage = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath, pathBuffer));
	backgroundImage = new Panel(areaWidth, areaHeight, screenTexCoords, screenTexImage, this);

	// Update Projection and View Matrix
	UpdateProjection();
}

ShortcutsPopupScreen::~ShortcutsPopupScreen()
{
	// Delete the background
	delete backgroundImage;
}

void ShortcutsPopupScreen::Render()
{
	// Render the background
	backgroundImage->Render();
}