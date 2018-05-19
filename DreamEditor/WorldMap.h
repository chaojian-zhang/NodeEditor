//#pragma once
//#include "InterfaceElement.h"
//#include "RenderableString.h"
//#include "RenderableRoundCornerFrame.h"
//#include "RenderableImageRect.h"
//#include "RenderableCirclesInstance.h"
//
//class WorldMap : public InterfaceElement
//{
//public:
//	WorldMap(unsigned int width, unsigned int height, Interface* parent);
//	~WorldMap();
//
//	// Interface Implementation
//	virtual void UpdatePosition(int newx, int newy);
//	virtual void Render();
//	virtual void OnMouseOver(double xpos, double ypos);
//	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos);
//	virtual void OnKeyboardButton(int key, int scancode, int action, int mods);
//	virtual void OnTextInput(unsigned short* string, unsigned int numChars);
//	virtual void OnDropWindow(int count, const char** paths);
//	virtual void OnMouseLeave();
//
//private:
//	// Bookeeping
//
//private:
//	// Renderables: A Map editor will show one major section, and fractions of other smaller sections, so a total of 9 sections at a time
//	// ----------- Horizontal Marks
//	RenderableString* horiMark1;
//	RenderableString* horiMark2;
//	RenderableString* horiMark3;
//	RenderableRoundCornerFrame* horiSqaure;
//	// ----------- Vertical Marks
//	RenderableString* vertMark1;
//	RenderableString* vertMark2;
//	RenderableString* vertMark3;
//	RenderableRoundCornerFrame* vertSquare;
//	// ----------- Map View
//	RenderableImageRect* section1;
//	RenderableImageRect* section2;
//	RenderableImageRect* section3;
//	RenderableImageRect* section4;
//	RenderableImageRect* section5;
//	RenderableImageRect* section6;
//	RenderableImageRect* section7;
//	RenderableImageRect* section8;
//	RenderableImageRect* section9;
//	RenderableRoundCornerFrame* mapSquare;
//	// ----------- Indicator Batch
//	RenderableCirclesInstance* indicators;
//};