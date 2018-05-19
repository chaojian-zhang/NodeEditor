#include "AboutPopupScreen.h"
#include "Colors.h"
#include "MaterialTextures.h"
#include "SystemWrapper.h"
#include "Constants.h"

#define AboutPop_HorizontalPadding 600
#define AboutPop_VerticalPadding 440

unsigned int AboutPopupScreen::areaWidth = 850U;
unsigned int AboutPopupScreen::areaHeight = 500U;
float AboutPopupScreen::screenTexCoords[8] = { 
	850.0f / 2048.0f, 1024.0f/ 2048.0f,
	1700.0f / 2048.0f, 1024.0f / 2048.0f,
	1700.0f / 2048.0f, 1524.0f / 2048.0f,
	850.0f / 2048.0f, 1524.0f / 2048.0f };
GLuint AboutPopupScreen::screenTexImage = NULL;

AboutPopupScreen::AboutPopupScreen()
{
	// Create Interface Elements
	// A Label that indicates the purpose of the Screen
	original = new TextLabel((unsigned short*)Interface_AboutScreen_Original, false, Color::LightGreyTextColor, this);
	labels.Add(original);
	// A Test Label That does nothing
	contact = new TextLabel((unsigned short*)Interface_AboutScreen_Contact, false, Color::LightGreyTextColor, this);
	labels.Add(contact);

	// Use a layout
	AutoExpandGridLayout layout(AboutPop_HorizontalPadding, -AboutPop_VerticalPadding, LayoutRowPadding, LayoutColumnPadding);
	// Iterate and add lines
	TextLabel* temp = (TextLabel*)(labels.First());
	while (temp)
	{
		layout.AddToNewRow(temp);
		temp = (TextLabel*)(labels.Next());
	}
	// Do a Dispatch
	layout.Dispatch();

	// Set up correct viewport size
	//viewportW = layout.layoutWidth + AboutPop_HorizontalPadding * 2;
	//viewportH = layout.layoutHeight + AboutPop_VerticalPadding * 2;
	viewportW = areaWidth;
	viewportH = areaHeight;

	// Set up a background image
	char pathBuffer[MAX_PATH];
	screenTexImage = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath, pathBuffer));
	backgroundImage = new Panel(areaWidth, areaHeight, screenTexCoords, screenTexImage, this);

	// Offset Z Height a bit to avoid being overlaped by other interfaces.
	//backgroundRect->OffsetPosition(0, 0, 0.2);
	// Iterate and adjust
	temp = (TextLabel*)(labels.First());
	while (temp)
	{
		temp->OffsetPosition(0, 0, 0.4);
		temp = (TextLabel*)(labels.Next());
	}

	// Update Projection and View Matrix
	UpdateProjection();
}

AboutPopupScreen::~AboutPopupScreen()
{
	// Delete Interface Elements
	// Delete the background
	delete backgroundImage;
	// Iterate and delete
	TextLabel* temp = (TextLabel*)(labels.First());
	while (temp)
	{
		delete temp;
		temp = (TextLabel*)(labels.Next());
	}
}

void AboutPopupScreen::Render()
{
	// Render the background
	backgroundImage->Render();
	// Iterate and Render
	TextLabel* temp = (TextLabel*)(labels.First());
	while (temp)
	{
		temp->Render();
		temp = (TextLabel*)(labels.Next());
	}
}