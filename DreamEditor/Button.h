#pragma once

#include "InterfaceElement.h"
#include "Panel.h"
#include "RenderableImageIcon.h"
#include "RenderableRoundCornerQuad.h"
#include "RenderableString.h"
class Interface;

// Semantics Matter
enum ColorScheme
{
	MenuButton,
	PopupButton
};

// Reminder: Input Abs OpenGl Second Quadrant Coordinates are relative to viewport Upper-Left corner, not window
// Renderables, and also The InterfaceElement itself's locations aren't determined at creation time, but later in UpdatePosition(), called by Interface Directly or GridLayout
class Button : public InterfaceElement
{
public:
	Button(unsigned short* faceString, Interface* inParent, ColorScheme inColorScheme = MenuButton);	// NULL terminated UTF16
	Button(unsigned short* faceString, Interface* inParent, bool border, float inCornerSize, ColorScheme inColorScheme = MenuButton);	// NULL terminated UTF16
	Button(float* iconCoords, unsigned short* faceString, GLuint iconTexture, Interface* inParent);	// For those with icons
	~Button();	// Delete Allocated Memory

	// Transformation Update
	virtual void UpdatePosition(int newx, int newy);	// Need to update all sub-renderable objects' transformations
	virtual void OffsetPosition(int dx, int dy, float dz);
	// Render
	virtual void Render();
	// Mouse: : Change Color according to current state - Hovered, Selected, Normal
	virtual void OnMouseOver(double xpos, double ypos);	// Hover Color
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos);	// LMB only, change color and state
	virtual void OnMouseLeave();
	// Not used
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods){};
	virtual void OnTextInput(unsigned short* text, unsigned int numChars){};
	virtual void OnDropWindow(int count, const char** paths){};

protected:
	// State Variable
	bool bHovered = false;
	bool bSelected = false;
	bool bBorder = true;
	ColorScheme colorScheme = MenuButton;

	// Global Default Style
	static unsigned int PaddingX;
	static unsigned int PaddingY;
	static unsigned int CornerSize;
	static unsigned int FontSize;	// It would Require at least 16 pixels to display CHN chars clearly, and only 8 pixels to display ENG clearly, at least in our font
	static unsigned int IconSize;

	// Global Default Style 1: MenuButton
	static float TextColor_Scheme1[3];
	static float BorderColor_Scheme1[3];
	static float BorderHoverColor_Scheme1[3];
	// Do not need a BorderSelectColor
	static float FaceColor_Scheme1[3];
	static float FaceHoverColor_Scheme1[3];
	static float FaceSelectColor_Scheme1[3];

	// Global Default Style 2: PopupButton
	static float TextColor_Scheme2[3];
	static float BorderColor_Scheme2[3];
	static float BorderHoverColor_Scheme2[3];
	static float FaceColor_Scheme2[3];
	static float FaceHoverColor_Scheme2[3];
	static float FaceSelectColor_Scheme2[3];

protected:
	// Renderables
	RenderableImageIcon* primaryIcon = NULL;	// Useful for MeshCreation Popup Menu buttons
	RenderableRoundCornerQuad* buttonFace;	// "Face" of a button is important...
	RenderableString* text;
};