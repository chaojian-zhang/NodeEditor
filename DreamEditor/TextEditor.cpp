#include "TextEditor.h"
#include "GraphManager.h"

unsigned int TextEditor::XPadding = 4;
unsigned int TextEditor::YPadding = 4;

TextEditor::TextEditor(float w, float h, unsigned short* initialText, unsigned int numChars, unsigned int fSize, float inColor[3], Interface* inParent, bool singleLine, bool fixWidth, bool fixHeight, bool useReference)
{
	parent = inParent;
	text = new TextContainer(inParent, initialText, numChars, fSize, inColor, w, h, singleLine, false, true, fixWidth, fixHeight, NULL, useReference);

}
TextEditor::~TextEditor()
{
	delete text;
}

// Base Interface Implementation
void TextEditor::UpdatePosition(int newx, int newy)
{
	InterfaceElement::UpdatePosition(newx, newy);

	// __PEnding: TextScroll
	text->Translation(newx + XPadding, newy - (int)YPadding, 0.1);
}

void TextEditor::Render()
{
	text->Render();
}
void TextEditor::OnMouseOver(double xpos, double ypos)
{
	/*if (xpos > x + XPadding && ypos < y - YPadding)*/
	if (xpos > x && ypos < y)
	{
		GraphManager::SetCursorIcon(CursorIcon::TextCursor);
	}
	else
	{
		GraphManager::SetCursorIcon(CursorIcon::NormalCursor);
	}

	bMouseOverNotified = true;
}
void TextEditor::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	text->OnMouseButton(button, action, mods, xpos - x, ypos - y);	// Remember that TextArea is 2D, so we do not need to bother ray collision here
}
void TextEditor::OnKeyboardButton(int key, int scancode, int action, int mods)
{
	text->OnKeyboardButton(key, scancode, action, mods);
}
void TextEditor::OnTextInput(unsigned short* string, unsigned int numChars)
{
	text->OnTextInput(string, numChars);
}
void TextEditor::OnDropWindow(int count, const char** paths)
{
	// Update Data
	for (int i = 0; i < count; i++)
	{
		// Convert to U16
		std::wstring utf16 = SystemFunctions::ConvertU8toU16(paths[i]);

		text->OnTextInput((unsigned short*)utf16.c_str(), utf16.length());
	}
}
void TextEditor::OnMouseLeave()
{
	GraphManager::SetCursorIcon(CursorIcon::NormalCursor);
	bMouseOverNotified = false;
}

unsigned int TextEditor::GetTextHeight()
{
	return text->GetBoundaryHeight();
}

//void TextEditor::UpdateContent(unsigned short* newText, unsigned int numChars)
//{
//	// Update content using a not so elegant way: Select all, then add in new content
//	OnKeyboardButton(GLFW_KEY_A, 0, GLFW_PRESS, GLFW_MOD_CONTROL);
//	OnTextInput(newText, numChars);
//}

void TextEditor::AssociateStringData(StringData* newString)
{
	text->Associate(newString);
}