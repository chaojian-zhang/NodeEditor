#include "Button.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include "Interface.h"

// Static Variables
unsigned int Button::PaddingX = 4;
unsigned int Button::PaddingY = 2;
unsigned int Button::CornerSize = 2U;	// 4 will cause some inaccuracy, 6 is good
unsigned int Button::FontSize = 16U;
unsigned int Button::IconSize = 16U;
float Button::TextColor_Scheme1[3] = { 0.784, 0.784, 0.784 };
float Button::BorderColor_Scheme1[3] = { 0.176, 0.176, 0.176};
float Button::BorderHoverColor_Scheme1[3] = { 0.863, 0.863, 0.863 };
float Button::FaceColor_Scheme1[3] = { 0.2, 0.2, 0.2};
float Button::FaceHoverColor_Scheme1[3] = { 0.243, 0.243, 0.243};
float Button::FaceSelectColor_Scheme1[3] = { 0.863, 0.863, 0.863 };

float Button::TextColor_Scheme2[3] = { 0.243f, 0.243f, 0.259f };
float Button::BorderColor_Scheme2[3] = { 0.635, 0.635, 0.635};
float Button::BorderHoverColor_Scheme2[3] = { 0.635, 0.635, 0.635 };
float Button::FaceColor_Scheme2[3] = { 0.788f, 0.788f, 0.788f };
float Button::FaceHoverColor_Scheme2[3] = { 0.635, 0.635, 0.635 };
float Button::FaceSelectColor_Scheme2[3] = { 0.863, 0.863, 0.863 };

Button::Button(unsigned short* faceString, Interface* inParent, ColorScheme inColorScheme)
	:colorScheme(inColorScheme)
{
	parent = inParent;

	switch (colorScheme)
	{
	case MenuButton:
		// Create Button Renderables
		text = new RenderableString(parent, faceString, GetStringLength(faceString), TextColor_Scheme1, FontSize, false);

		// Set Width and Height
		width = text->GetStringWidth() + PaddingX * 2;
		height = text->GetStringHeight() + PaddingY * 2;

		// Create Button Face
		buttonFace = new RenderableRoundCornerQuad(parent, width, height, false, BorderColor_Scheme1, FaceColor_Scheme1, CornerSize, false, false);	// Apply padding space reservations; Default no border
		break;
	case PopupButton:
		// Create Button Renderables
		text = new RenderableString(parent, faceString, GetStringLength(faceString), TextColor_Scheme2, FontSize, false);

		// Set Width and Height
		width = text->GetStringWidth() + PaddingX * 2;
		height = text->GetStringHeight() + PaddingY * 2;

		// Create Button Face
		buttonFace = new RenderableRoundCornerQuad(parent, width, height, false, BorderColor_Scheme2, FaceColor_Scheme2, CornerSize, false, false);	// Apply padding space reservations; Default no border
		break;
	default:
		break;
	}
}

Button::Button(unsigned short* faceString, Interface* inParent, bool border, float inCornerSize, ColorScheme inColorScheme)
	:colorScheme(inColorScheme), bBorder(border)
{
	parent = inParent;

	switch (colorScheme)
	{
	case MenuButton:
		// Create Button Renderables
		text = new RenderableString(parent, faceString, GetStringLength(faceString), TextColor_Scheme1, FontSize, false);

		// Set Width and Height
		width = text->GetStringWidth() + PaddingX * 2;
		height = text->GetStringHeight() + PaddingY * 2;

		// Create Button Face
		buttonFace = new RenderableRoundCornerQuad(parent, width, height, border, BorderColor_Scheme1, FaceColor_Scheme1, inCornerSize, false, false);	// Apply padding space reservations; Default no border
		break;
	case PopupButton:
		// Create Button Renderables
		text = new RenderableString(parent, faceString, GetStringLength(faceString), TextColor_Scheme2, FontSize, false);

		// Set Width and Height
		width = text->GetStringWidth() + PaddingX * 2;
		height = text->GetStringHeight() + PaddingY * 2;

		// Create Button Face
		buttonFace = new RenderableRoundCornerQuad(parent, width, height, border, BorderColor_Scheme2, FaceColor_Scheme2, inCornerSize, false, false);	// Apply padding space reservations; Default no border
		break;
	default:
		break;
	}
}

Button::Button(float* iconCoords, unsigned short* faceString, GLuint iconTexture, Interface* inParent)
{
	parent = inParent;

	// Create Button Renderables
	text = new RenderableString(false, faceString, GetStringLength(faceString), TextColor_Scheme1, FontSize, false);
	primaryIcon = new RenderableImageIcon(parent, IconSize, iconCoords, iconTexture ? iconTexture : GraphManager::shaderResource->GetAppIconMap());	// Icon share the same height thus width as texts
	
	// Set Width and Height
	width = text->GetStringWidth() + PaddingX * 3 + IconSize;	// 2 paddings for text, another 1 for icon
	height = text->GetStringHeight() + PaddingY * 2;

	// Create Button Face
	buttonFace = new RenderableRoundCornerQuad(parent, width, height, false, BorderColor_Scheme1, FaceColor_Scheme1, CornerSize, false, false);	// Apply padding space reservations
}

Button::~Button()
{
	if (primaryIcon != NULL)
	{
		delete primaryIcon;
	}
	delete buttonFace;
	delete text;
}

// Interface Implementation
void Button::UpdatePosition(int newx, int newy)
{
	InterfaceElement::UpdatePosition(newx, newy);

	// Update Renderable Locations
	// Apply a padding between renderables
	buttonFace->Translation(x, y, 0.05);
	if (primaryIcon != NULL)
	{
		primaryIcon->Translation(x + PaddingX, y - (int)PaddingY, 0.1);	// remember we are in openGL second quadrant
	}

	text->Translation(x + PaddingX + (primaryIcon ? IconSize : 0), y - (int)PaddingY, 0.1);
}

void Button::OffsetPosition(int dx, int dy, float dz)
{
	InterfaceElement::OffsetPosition((float)dx, (float)dy, dz);

	buttonFace->Translate(dx, dy, dz);
	if(primaryIcon!=NULL)primaryIcon->Translate(dx, dy, dz);
	text->Translate(dx, dy, dz);
}

void Button::Render()
{
	if (primaryIcon) primaryIcon->Render();
	buttonFace->Render();
	text->Render();
}

void Button::OnMouseOver(double xpos, double ypos)
{
	bMouseOverNotified = true;

	if (!bSelected && !bHovered)	// if Selected, then do nothing; if already hovered, then do nothing
	{
		bHovered = true;
		// Set Button Quad Color To Hovered Color
		switch (colorScheme)
		{
		case MenuButton:
			buttonFace->UpdateProperty(bBorder, BorderHoverColor_Scheme1, FaceHoverColor_Scheme1);
			break;
		case PopupButton:
			buttonFace->UpdateProperty(bBorder, BorderHoverColor_Scheme2, FaceHoverColor_Scheme2);
			break;
		default:
			break;
		}
	}
}

void Button::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	bSelected = true;
	// Change Button Quad Color to Selection Color
	switch (colorScheme)
	{
	case MenuButton:
		buttonFace->UpdateProperty(bBorder, BorderHoverColor_Scheme1, FaceSelectColor_Scheme1);
		break;
	case PopupButton:
		buttonFace->UpdateProperty(bBorder, BorderHoverColor_Scheme2, FaceSelectColor_Scheme2);
		break;
	default:
		break;
	}
}

void Button::OnMouseLeave()
{
	if (bHovered)
	{
		bHovered = false;
		bSelected = false;
		// Set Button Quad Color to Normal Color
		switch (colorScheme)
		{
		case MenuButton:
			buttonFace->UpdateProperty(false, BorderColor_Scheme1, FaceColor_Scheme1); // Hide Border as well
			break;
		case PopupButton:
			buttonFace->UpdateProperty(false, BorderColor_Scheme2, FaceColor_Scheme2); // Hide Border as well
			break;
		default:
			break;
		}
	}

	bMouseOverNotified = false;
}