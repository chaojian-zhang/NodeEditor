#include "TextArea.h"
#include  "Constants.h"
#include "Colors.h"

int TextArea::CornerSize = 2;
float TextArea::FrameColor[3] = { 0.408, 0.408, 0.408};
//float TextArea::FrameHoverColor[3] = { 0.619, 0.619, 0.619};
float TextArea::FrameHoverColor[3] = { 0.445, 0.339, 0.769};
float TextArea::BackGroundColor[3] = { 0.243, 0.243, 0.243};

TextArea::TextArea(float w, float h, unsigned short* initialText, unsigned int numChars, Interface* inParent, bool useReference)
	:TextEditor(w, h, initialText, numChars, InterfaceConstant::DefaultFontSize, Color::DefaultTextColor,
	inParent, false, true, true, useReference)
{
	XPadding = 4;
	YPadding = 4;

	width = w + XPadding * 2;
	height = h + YPadding * 2;

	background = new RenderableRoundCornerQuad(inParent, width, height, true, FrameColor, BackGroundColor, CornerSize, false, false);
}

TextArea::~TextArea()
{
	delete background;
}

// Interface Implmentation
void TextArea::UpdatePosition(int newx, int newy)
{
	TextEditor::UpdatePosition(newx, newy);
	background->Translate(newx, newy, 0.01);
}

void TextArea::Render()
{
	background->Render();
	TextEditor::Render();
}

void TextArea::OnMouseOver(double xpos, double ypos)
{
	TextEditor::OnMouseOver(xpos, ypos);
	background->UpdateProperty(true, FrameHoverColor, BackGroundColor);
}

void TextArea::OnMouseLeave()
{
	TextEditor::OnMouseLeave();
	background->UpdateProperty(true, FrameColor, BackGroundColor);
}