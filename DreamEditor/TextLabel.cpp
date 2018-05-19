#include "TextLabel.h"
#include  "Constants.h"
#include "Colors.h"
#include "MathElegent.h"

TextLabel::TextLabel(unsigned short* initialText, bool dynamic, Interface* inParent)
{
	text = new RenderableString(inParent, initialText, GetStringLength(initialText),
		Color::DefaultTextColor, InterfaceConstant::DefaultFontSize, dynamic, false);

	width = text->GetStringWidth();
	height = text->GetStringHeight();
	parent = inParent;
}

TextLabel::TextLabel(unsigned short* initialText, bool dynamic, float inColor[3], Interface* inParent)
{
	text = new RenderableString(inParent, initialText, GetStringLength(initialText),
		inColor, InterfaceConstant::DefaultFontSize, dynamic, false);

	width = text->GetStringWidth();
	height = text->GetStringHeight();
	parent = inParent;
}

TextLabel::~TextLabel()
{
	delete text;
}

// Interface Implmentation
void TextLabel::UpdatePosition(int newx, int newy)
{
	InterfaceElement::UpdatePosition(newx, newy);
	text->Translation(newx, newy, 0.1);
}

void TextLabel::OffsetPosition(int dx, int dy, float dz)
{
	InterfaceElement::OffsetPosition(dx, dy, dz);
	text->Translate((float)dx, (float)dy, dz);
}

void TextLabel::Render()
{
	text->Render();
}

void TextLabel::UpdateContent(unsigned short* newText, float inColor[3])
{
	text->UpdateBuffers_Line(newText, GetStringLength(newText), InterfaceConstant::DefaultFontSize);

	width = text->GetStringWidth();
	height = text->GetStringHeight();
}

void TextLabel::UpdateTransparancy(float value)
{
	text->UpdateTransparancy(value);
}