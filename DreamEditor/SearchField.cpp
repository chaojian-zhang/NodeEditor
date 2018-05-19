#include "SearchField.h"
#include "GraphManager.h"
#include "Constants.h"
#include "Colors.h"

// Static Variables
int SearchField::CornerSize = 2;
int SearchField::IconSize = 16U;	// Same as default font size
float SearchField::FrameColor[3] = { 0.408f, 0.408f, 0.408f };
float SearchField::FrameHoverColor[3] = { 0.445f, 0.339f, 0.769f };
float SearchField::BackGroundColor[3] = { 0.23f, 0.23f, 0.23f};
float SearchField::SearchIconCoords[8] = { 
	0.0f, 993.0f / 2048.0f, 
	31.0f / 2048.0f, 993.0f / 2048.0f, 
	31.0f / 2048.0f, 1024.0f / 2048.0f, 
	0.0f, 1024.0f / 2048.0f };

SearchField::SearchField(unsigned short* initialString, unsigned int numChars, Interface* inParent)
	:TextEditor(160, 0, (unsigned short*)L"Debug String", 12, InterfaceConstant::DefaultFontSize, Color::DefaultTextColor, inParent, true, true, false, false)
{
	XPadding = 4;
	YPadding = 2;

	// Set Correct Width and height of interface element, overriding TextField ones
	width = InterfaceConstant::DefaultTextFieldPixelWidth + IconSize + XPadding * 3;
	height = InterfaceConstant::DefaultFontSize > IconSize ? InterfaceConstant::DefaultFontSize : IconSize + YPadding * 2;

	// Initialize Renderables
	frame = new RenderableRoundSidesQuad(inParent, width, height, true, FrameColor, BackGroundColor, false, false);
	icon = new RenderableImageIcon(inParent, IconSize, SearchIconCoords, GraphManager::shaderResource->GetAppIconMap());
}

SearchField::~SearchField()
{
	delete frame;
	delete icon;
}

// Interface modification
void SearchField::UpdatePosition(int newx, int newy)
{
	TextEditor::UpdatePosition(newx + XPadding + IconSize, newy);
	frame->Translate(newx, newy, 0.01);
	icon->Translate(newx + XPadding, newy - (int)YPadding, 0.1);	// Notive here if we do not explicitly convert YPadding to interger type, it will cause very big value
}

// Render Request
void SearchField::Render()
{
	// Render Frame First
	frame->Render();

	// Then Render Text
	//// Limit Range of String Renderable display
	//// -> Just Use viewport to limit display range isn't gonnad work, since our project2DView matrix depend on the viewport
	//// First Hack Renderbale View Matrix
	//glm::mat4x4 temp = parent->project2DView;	// Keep a backup
	//glm::mat4x4 translation = glm::translate<float>(glm::mat4(1), glm::vec3(-(int)(x + XPadding + IconSize + XPadding), 0, 0));
	//parent->project2DView = glm::ortho<float>(0, DisplayWidth, -(int)parent->viewportH, 0, 0, 100) * GraphManager::View2D;
	//parent->project2DView *= translation;	// By the way no need to keep the above lines a seperate class member, since it won't take too much computing power while keep the code clean in a sense that those things do not get spreaded all over the class source code
	//// Set Viewport to Limited Range
	//glViewport(parent->viewportX + x + XPadding + IconSize + XPadding, parent->viewportY, DisplayWidth, parent->viewportH);	// Limit Horizontal Range of rendering to the text field
	//// Do the render
	//textRenderable->Render();
	//// Set Hacking Back
	//parent->project2DView = temp;
	//// Set Viewport back
	//glViewport(parent->viewportX, parent->viewportY, parent->viewportW, parent->viewportH);
	TextEditor::Render();

	// Second last Render The Search Icon
	icon->Render();
}

// Mouse Event handlers
void SearchField::OnMouseOver(double xpos, double ypos)
{
	// Make it more precise
	TextEditor::OnMouseOver(xpos + XPadding + IconSize, ypos);

	// Change Border Color
	frame->UpdateProperty(true, FrameHoverColor, BackGroundColor);
}

void SearchField::OnMouseLeave()
{
	TextEditor::OnMouseLeave();
	frame->UpdateProperty(true, FrameColor, BackGroundColor);
}

void SearchField::OnTextInput(unsigned short* string, unsigned int numChars)
{
	TextEditor::OnTextInput(string, numChars);

	// Ask canvas to do instant parallel search
}