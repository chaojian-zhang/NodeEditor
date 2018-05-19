//#pragma once
//#include "InterfaceElement.h"
//#include "TextContainer.h"
//#include "Renderable2DRoundCornerQuad.h"
//
//// A Text Field with single line cornered box frame
//class TextBox : public InterfaceElement
//{
//public:
//	TextBox(Interface* inParent, unsigned short* initialString, unsigned int numChars);	// Override width and height property
//	~TextBox();	// Destroy allocated memory
//
//	// Interface Modification
//	virtual void UpdatePosition(int newx, int newy);
//	virtual void Render();
//	virtual void OnMouseOver(double xpos, double ypos);
//	virtual void OnMouseLeave();
//
//private:
//	// Static Variables
//	static int CornerSize;
//	static unsigned int XPadding;
//	static unsigned int YPadding;
//	static float FrameColor[3];
//	static float FrameHoverColor[3];
//	static float BackGroundColor[3];
//
//private:
//	// Renderables
//	TextContainer* fieldText;
//	Renderable2DRoundCornerQuad* frame;
//};