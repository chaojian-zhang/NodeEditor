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
	// Create Interface Elements: Lines of shortcuts
	/*line1 = new TextLabel((unsigned short*)L"This is a line         Each Line can have two columns", false, Color::LightGreyTextColor, this);
	labels.Add(line1);
	line2 = new TextLabel((unsigned short*)L"This is a line         Each Line can have two columns", false, Color::LightGreyTextColor, this);
	labels.Add(line2);
	line3 = new TextLabel((unsigned short*)L"This is a line         Each Line can have two columns", false, Color::LightGreyTextColor, this);
	labels.Add(line3);*/

	// Use a layout
	//AutoExpandGridLayout layout(AboutPop_HorizontalPadding, -AboutPop_VerticalPadding, LayoutRowPadding, LayoutColumnPadding);
	// Iterate and add lines
	//TextLabel* temp = (TextLabel*)(labels.First());
	//while (temp)
	//{
	//	layout.AddToNewRow(temp);
	//	temp = (TextLabel*)(labels.Next());
	//}
	// Do a Dispatch
	//layout.Dispatch();

	// Set up correct viewport size
	//viewportW = layout.layoutWidth + AboutPop_HorizontalPadding * 2;
	//viewportH = layout.layoutHeight + AboutPop_VerticalPadding * 2;
	viewportW = areaWidth;
	viewportH = areaHeight;

	// Set up a background image
	char pathBuffer[MAX_PATH];
	screenTexImage = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath, pathBuffer));
	backgroundImage = new Panel(areaWidth, areaHeight, screenTexCoords, screenTexImage, this);

	// Offset Z Height abit to avoid being overlaped by other interfaces.
	//backgroundRect->OffsetPosition(0, 0, 0.2);
	// Iterate and adjust
	/*temp = (TextLabel*)(labels.First());
	while (temp)
	{
		temp->OffsetPosition(0, 0, 0.4);
		temp = (TextLabel*)(labels.Next());
	}*/

	// Update Projection and View Matrix
	UpdateProjection();
}

ShortcutsPopupScreen::~ShortcutsPopupScreen()
{
	// Delete Interface Elements
	// Delete the background
	delete backgroundImage;
	// Iterate and delete
	/*TextLabel* temp = (TextLabel*)(labels.First());
	while (temp)
	{
		delete temp;
		temp = (TextLabel*)(labels.Next());
	}*/
}

void ShortcutsPopupScreen::Render()
{
	// Render the background
	backgroundImage->Render();
	// Iterate and Render
	//TextLabel* temp = (TextLabel*)(labels.First());
	//while (temp)
	//{
	//	temp->Render();
	//	temp = (TextLabel*)(labels.Next());
	//}
}