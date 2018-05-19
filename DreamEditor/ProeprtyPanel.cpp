#include "PropertyPanel.h"
#include "GraphManager.h"
#include "Constants.h"
#include "MaterialTextures.h"

int PropertyPanel::LayoutBeginPositionX = 32;
int PropertyPanel::LayoutBeginPositionY = -24;
unsigned int PropertyPanel::LayoutRowPadding = 8U;
unsigned int PropertyPanel::LayoutColumnPadding = 4U;
unsigned int PropertyPanel::TextContainerWidth = 280U;
GLuint PropertyPanel::PropertyPanelTexImage = NULL;
unsigned int PropertyPanel::PanelWidth = 384U;	// The original design size 512x801 is too big on screen
unsigned int PropertyPanel::PanelHeight = 600U;

PropertyPanel::PropertyPanel(float* textureCoordinates)
{
	// Set Viewport Dimensions
	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	char pathBuffer[MAX_PATH];
	PropertyPanelTexImage = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath, pathBuffer));

	// Scale Default panel size for smaller screens
	if (GraphManager::windowWidth < 1920)
	{
		PanelWidth = 315U;
		PanelHeight = 491U;
	}

	// Create a background iamge
	backgroundImage = new Panel(PanelWidth, PanelHeight, textureCoordinates, PropertyPanelTexImage, this);
}

PropertyPanel::~PropertyPanel()
{
	delete backgroundImage;
}

void PropertyPanel::Hide()
{
	GraphManager::currentPropertyPanel = NULL;
}

void PropertyPanel::Show()
{
	GraphManager::currentPropertyPanel = this;
}

// Partial Interface Implementation
void PropertyPanel::OnResizeFramebuffer(int width, int height)
{
	// Define its viewport
	// Notice these are relative to Lower-Left corner of the window
	viewportX = 25U;
	viewportY = GraphManager::windowHeight / 2 - PanelHeight / 3;
	viewportW = PanelWidth;
	viewportH = PanelHeight;

	// Update Projection and View
	UpdateProjection();
}