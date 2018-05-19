#pragma once
#include <GL\glew.h>
#include "Interface.h"
#include "Panel.h"

class PropertyPanel : public Interface
{
public:
	PropertyPanel(float* textureCoordinates);
	~PropertyPanel();

	// Partial Interface Implementation
	virtual void OnResizeFramebuffer(int width, int height);
	virtual void Render(){ backgroundImage->Render(); };

	// Property Panel Display
	void Show();
	void Hide();
	void Transparency(float value){ backgroundImage->SetTransparency(value); };

protected:
	static int LayoutBeginPositionX;
	static int LayoutBeginPositionY;
	static unsigned int LayoutRowPadding;
	static unsigned int LayoutColumnPadding;
	static unsigned int TextContainerWidth;	// If Any
	//static float PropertyPanelTexCoords[8];
	static GLuint PropertyPanelTexImage;

	// Floating Panel Definition
	static unsigned int PanelWidth;
	static unsigned int PanelHeight;

private:
	Panel* backgroundImage;
};