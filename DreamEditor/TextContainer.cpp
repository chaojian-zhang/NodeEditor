#include "TextContainer.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include "Constants.h"
#include "Colors.h"
#include "SystemWrapper.h"

// Position Cursors
//unsigned int lineNumber;
//float linePixelWidth;
//GetLineInfoAt(numChars, NULL, &lineNumber, &linePixelWidth);
//cursor0->UpdateLocation(XPadding + CornerSize + linePixelWidth, lineNumber * GraphManager::font->common->lineHeight * fontScale, 0);
//cursor1->UpdateLocation(XPadding + CornerSize + linePixelWidth, lineNumber * GraphManager::font->common->lineHeight * fontScale, 0);

float TextContainer::highlightColor[3] = {0.2f, 0.6f, 1.0f};

TextContainer::TextContainer(Interface* inParent, unsigned short* initialString, unsigned int numChars,
	unsigned int fSize, float color[3], float areaWidth, float areaHeight, bool singleLine, bool d3, bool dynamic, bool fixWidth, bool fixHeight, StringData* data, bool useReference)
	:parent(inParent), boundaryWidth(areaWidth), boundaryHeight(areaHeight), b3D(d3), fontSize(fSize), bSingleLine(singleLine), bFixWidth(fixWidth), bFixHeight(fixHeight)
{
	// Set Basic Parameters
	memcpy(textColor, color, sizeof(float) * 3);
	fontScale = (float)fontSize / GraphManager::font->info->fontSize * (b3D ? InterfaceConstant::PixelToMeterScaleFactor : 1);

	// Initialize Member Variables
	cursorPosition0 = cursorPosition1 = numChars;

	// Initialize Interface Element Data
	if (data || useReference)	// If initial referenced string data is provided, or intended to be used this way, then mark bUsingReferenceData
	{
		bUsingReferencedData = true;

		// If referenced data is available, then replace it with initial string
		if (data)
		{
			textData = data;
			if (numChars != 0)
			{
				textData->ReplaceSubString(0, textData->GetStringNumChars(), initialString, numChars);
			}
		}
		// Otherwise initial string isn't used
		else
		{
			textData = NULL;	// In this case
		}
	}
	else
	{
		textData = new StringData(initialString, numChars);
	}

	// Initialize Interface Element Renderabls
	if (bSingleLine)
	{
		textRenderable = new RenderableString(parent, initialString, numChars, textColor, fontSize, dynamic, b3D);
	}
	else
	{
		textRenderable = new RenderableString(parent, initialString, numChars, textColor, areaWidth, fontSize, dynamic, b3D);
	}

	cursorLast = new RenderableLine(parent, b3D ? fontScale*2 : 1, GraphManager::font->common->base * fontScale, textColor, false);	// FOr 3D use fontScale*2 to make it visible from a longer distance
	cursorHighlight = new RenderableCursorHighlight(parent, highlightColor);
	cursorHighlight->UpdateProperty(0.5, NULL);

	// Update fieldWidth and Height
	// __Deebug__ There seems a bug here in the calculation
	if (bSingleLine&& !bFixWidth)// Multi-Line must be fixed width
	{
		boundaryWidth = textRenderable->GetStringWidth();
	}
	if (!bFixHeight)
	{
		boundaryHeight = textRenderable->GetStringHeight();
	}

	// Create Stencil Mask
	stencilMask = new RenderableRoundCornerQuad(parent, boundaryWidth + (b3D ? 0.01 : 2), boundaryHeight, false, Color::DebugColor_Red, Color::DebugColor_Yel, 0, true, true);
}

TextContainer::~TextContainer()
{
	if (textData && !bUsingReferencedData)
	{
		delete textData;
	}
	if (textRenderable)
	{
		delete textRenderable;
	}
	if (cursorLast)
	{
		delete cursorLast;
	}
	if (cursorHighlight)
	{
		delete cursorHighlight;
	}
	if (stencilMask)
	{
		delete stencilMask;
	}
}

// Interface Functions
void TextContainer::AddCharacters(unsigned short* string, unsigned int numChars)
{
	// Update String Data
	if (cursorPosition0 == cursorPosition1)
	{
		textData->AddToString(cursorPosition0, string, numChars);
	}
	else
	{
		textData->ReplaceSubString(cursorPosition0, cursorPosition1, string, numChars);
	}

	// Update Cursor Position
	// __Pending add support for field scroll and clamping
	cursorPosition0 = cursorPosition1 = Mini(cursorPosition0, cursorPosition1) + numChars;

	// Update Renderables
	UpdateRenderables();
}

void TextContainer::DeleteCharactersBackward()
{
	// Update String Data
	if (cursorPosition0!= 0 && cursorPosition0 == cursorPosition1)
	{
		if (textData->DeleteBackward(cursorPosition0))
		{
			cursorPosition1 = cursorPosition0 = cursorPosition1 - 1;
		}
	}
	else
	{
		textData->DeleteSubString(cursorPosition0, cursorPosition1);
		cursorPosition1 = cursorPosition0 = Mini(cursorPosition0, cursorPosition1);
	}

	// Update Renderables
	UpdateRenderables();
}

void TextContainer::DeleteCharactersForward()
{
	// Update String Data
	if (cursorPosition0 == cursorPosition1)
	{
		if (textData->DeleteForward(cursorPosition0))
		{
			cursorPosition1 = cursorPosition0;	// i.e. do not change position
		}
	}
	else
	{
		textData->DeleteSubString(cursorPosition0, cursorPosition1);
		cursorPosition1 = cursorPosition0 = Mini(cursorPosition0, cursorPosition1);
	}

	// Update Renderables
	UpdateRenderables();
}

void TextContainer::GetLineInfoAt(unsigned int numChars, unsigned int* lineCharNum, unsigned int* lineNum, float* linePixelWidth)
{
	if (numChars > textData->GetStringNumChars())
	{
		// Ignore request
		// __Debug__: Requre safer methods
		return;
	}
	else
	{
		unsigned int lineChar = 0;	// In 0 Indexes
		unsigned int lineNumber = 0;	// In 0 Indexes
		float lineWidth = 0;	// In pixels/meters
		for (unsigned int i = 0; i < numChars; i++)
		{
			// Easy Access
			unsigned short character = textData->GetString()[i];

			if (character == (unsigned int)L'\n')
			{
				if (bSingleLine)
				{// Actually for single line text this should never happen
					// Basiclaly Ignore new line
					lineWidth += 0;
					lineChar += 0;
					lineNumber += 0;
				}
				else
				{
					lineWidth = 0;
					lineChar = 0;
					lineNumber++;
				}
			}
			else if (character == (unsigned int)L'\t' || character < 32)
			{
				// Ignore: In out application we do not store \t characters
			}
			else
			{
				lineChar++;
				FontChar* charInfo = GraphManager::font->GetChar(character);
				lineWidth += charInfo ? (charInfo->xadvance * fontScale) : 0;	// Don't forget to apply scale~
				if (!bSingleLine && lineWidth > boundaryWidth)
				{
					lineWidth = charInfo ? (charInfo->xadvance * fontScale) : 0;	// Effectively begin on the next line
					lineChar = 0;
					lineNumber++;
				}
			}
		}

		// return values
		if (lineNum != NULL)
			*lineNum = lineNumber;
		if (lineCharNum != NULL)
			*lineCharNum = lineChar;
		if (linePixelWidth != NULL)
			*linePixelWidth = lineWidth;
	}
}

unsigned int TextContainer::GetNumChars(float x, float y)
{
	// __Pending Debug support for textScroll
	float scrolledY = y + (bSingleLine ? 0 : textScroll);
	float scrolledX = x + (bSingleLine ? -textScroll : 0);

	// Get Concrete Mouse Position in Line numbers, remember line number staring at index 0
	unsigned int mouseLineNumber = - scrolledY / (GraphManager::font->common->lineHeight * fontScale);
	// Debug_Temp
	cout << "[Debug]Current Mouse LIne: " << mouseLineNumber << endl;
	// Useful data
	unsigned int totalLines;
	GetLineInfoAt(textData->GetStringNumChars(), NULL, &totalLines);

	// Find Nearest cursor position, searching from beginning
	// Cases:
	// 1: Mouse Clicked outside text field - ignore mouse event
	// 2: Mouse clicked exactly inbetween/near some character - find nearest position to that character
	// 3: Mouse clicked on the blank area of a line - go to the end of that line
	// 4: Mouse clicked lower than current lines(i.e. in the blank area below current lines) - go to the last character of the last line
	// 5: Before first line the first character
	// Handle Case 1
	if (x < 0 || x > boundaryWidth || y > 0 || y < -boundaryHeight)
	{
		return cursorPosition1;
	}
	// Handle Case 4
	else if (mouseLineNumber > totalLines)
	{
		return textData->GetStringNumChars();
	}
	// Handle Case 5
	else if (textData->GetString()[0] != NULL && mouseLineNumber == 0 && scrolledX < GraphManager::font->GetChar(textData->GetString()[0])->width * fontScale / 2)
	{
		return 0;
	}
	// Handle Case 3 and 4
	else
	{
		float lineWidth = 0;
		unsigned int lineNumber = 0;
		for (unsigned int i = 0; i < textData->GetStringNumChars(); i++)
		{
			if (textData->GetString()[i] == (unsigned int)L'\n' && !bSingleLine)	// __Debug__: This might be too slow, consider expose the variable as a public
			{
				lineWidth = 0;
				lineNumber++;
			}
			else if (textData->GetString()[i] == (unsigned int)L'\t' || textData->GetString()[i] < 32)
			{
				// Ignore
			}
			else
			{
				FontChar* character = GraphManager::font->GetChar(textData->GetString()[i]);
				lineWidth += character? (character->xadvance * fontScale) : 0;
				if (lineWidth > boundaryWidth && !bSingleLine)
				{
					lineWidth = character->xadvance * fontScale;
					lineNumber++;
				}
			}

			// The existence of tab character impose some new chanllenge: the one immediately after a tab could be a nother non-recognizd character by BMFont, so we need to make sure it's not null. Actually the same thing should happen when two newlines are together

			// __Debug__: Seriously I've got no idea the below codes works
			// Case 2: Near some character
			// Left Half
			if (textData->GetString()[i + 1] != NULL && mouseLineNumber == lineNumber && GraphManager::font->GetChar(textData->GetString()[i + 1])  != NULL && GraphManager::font->GetChar(textData->GetString()[i + 1])->xadvance * fontScale / 2 + lineWidth >= scrolledX)
			{
				return i + 1;	// i is the location, i+1 is the number of characters
			}
			// Right Half
			if (mouseLineNumber == lineNumber && lineWidth >= scrolledX)
			{
				return i + 1;
			}
			// Case 3: Blank area at a line
			if (mouseLineNumber == lineNumber - 1)
			{
				// Now we know we didn't find a suitable position
				return i;	// Go back to end of last line
			}
		}

	}
}

void TextContainer::UpdateRenderables()
{
	// May not exist if we are in reference mode
	if (!textData)
	{
		return;
	}

	// Debug
	//textData->ReplaceSubString(0, textData->GetStringNumChars(), (unsigned short*)L"aaa", 3);	// This won't work because we didn't update cursor pos which is critical for correct formatting of renderable
	//textData->ReplaceSubString(0, 3, (unsigned short*)L"aaa", 3);	// this can work because it didn't hurt anything

	// Update Renderables
	if (bSingleLine)
	{
		textRenderable->UpdateBuffers_Line(textData->GetString(), textData->GetStringNumChars(), fontSize);
	}
	else
	{
		textRenderable->UpdateBuffers_Area(textData->GetString(), textData->GetStringNumChars(), boundaryWidth, fontSize);
	}
	if (bSingleLine && !bFixWidth)
	{
		boundaryWidth = textRenderable->GetStringWidth();
	}
	if (!bFixHeight)
	{
		boundaryHeight = textRenderable->GetStringHeight();
	}

	// Update Cursor
	UpdateCursors();

	// Update Stencil BUffer
	stencilMask->UpdateBuffer(boundaryWidth + (b3D ? 0.01 : 2), boundaryHeight, 0);
}

// Fundamental Functions
float TextContainer::GetBoundaryWidth()
{
	return boundaryWidth;
}

float TextContainer::GetBoundaryHeight()
{
	return boundaryHeight;
}

// Notice we are in World space
void TextContainer::Translation(float x, float y, float z)
{
	Tx = x;
	Ty = y;
	Tz = z;

	// Text
	textRenderable->Translation(x, y, z);

	// Cursor
	UpdateCursors();

	// Stencil
	stencilMask->Translation(x, y, z);
}

void TextContainer::Rotation(float x, float y, float z)
{
	// Text
	textRenderable->Rotation(x, y, z);

	// __Pending Debug: Curosr

	// Update Stencil

	cout << "Not Implemented!" << endl;
	__debugbreak();
}

void TextContainer::Scaling(float x, float y, float z)
{
	// Text

	// Curosr

	// Update Stencil

	cout << "Not Implemented!" << endl;
	__debugbreak();
}

void TextContainer::Translate(float dx, float dy, float dz)
{
	Tx += dx;
	Ty += dy;
	Tz += dz;

	// Text
	textRenderable->Translate(dx, dy, dz);

	// Cursor
	UpdateCursors();

	// Stencil
	stencilMask->Translate(dx, dy, dz);
}

void TextContainer::Rotate(float x, float y, float dz)
{
	// Text

	// Curosr

	// Update Stencil

	cout << "Not Implemented!" << endl;
	__debugbreak();
}

void TextContainer::Scale(float x, float y, float dz)
{
	// Text

	// Curosr

	// Update Stencil

	cout << "Not Implemented!" << endl;
	__debugbreak();
}

void TextContainer::UpdateCursors()
{
	// May not exist if we are in reference mode
	if (!textData)
	{
		return;
	}

	if (cursorLast)
	{
		// Get Basic Information about cursor 0
		unsigned int lineNumber0, lineNumber1;
		float lineWidth0, lineWidth1;
		GetLineInfoAt(cursorPosition0, NULL, &lineNumber0, &lineWidth0);

		// Text Scroll, subject to curosr 0
		// Useful Variables
		float lineHeight = ((int)lineNumber0 + 1) * GraphManager::font->common->lineHeight * fontScale;
		// If cursor1 is out of viewable range, then scroll the text
		if (bSingleLine && (lineWidth0 + textScroll > boundaryWidth || lineWidth0 + textScroll < 0) )
		{
			textScroll = boundaryWidth - lineWidth0 > 0 ? 0 : boundaryWidth - lineWidth0;

			// Update Text and Renderables
			textRenderable->Translation(Tx + textScroll, Ty, Tz);	// Relative to object space
			// Update Cursor 0
			//cursor0->Translation(Tx + lineWidth + textScroll, Ty - (int)lineNumber * GraphManager::font->common->lineHeight * fontScale, Tz + 0.01);
			// Update Cursor 1
			GetLineInfoAt(cursorPosition1, NULL, &lineNumber1, &lineWidth1);
			cursorLast->Translation(Tx + lineWidth1 + textScroll, Ty - (int)lineNumber1 * GraphManager::font->common->lineHeight * fontScale, Tz + 0.01);

			// Update CursorHighlight: SingleLine
			// Notice we may be sending a negative value as spanWidth, however this works since we treat cursorPosition0 as origin, thus when cursor0 is on the left of cursor1 it should be spanning towards left rather than right
			// Culling is disabled for double face rendering of Textual elements in our application so no worry it will be culled
			// But any way this isn't expected so we'd better be more accurate
			cursorHighlight->UpdateBuffer_SingleLine(glm::abs(lineWidth1 - lineWidth0), GraphManager::font->common->lineHeight * fontScale);
			cursorHighlight->Translation(Tx + (cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1) + textScroll, Ty - (int)(cursorPosition0 < cursorPosition1 ? lineNumber0 : lineNumber1)* GraphManager::font->common->lineHeight * fontScale, Tz + 0.01);	// Take position of the cursor that is nearest the beginning
		}
		else if (bFixWidth && bFixHeight && (lineHeight + textScroll > boundaryHeight || lineHeight + textScroll < 0))
		{
			if (lineHeight + textScroll < 0)
			{
				textScroll += GraphManager::font->common->lineHeight * fontScale;	// Notice Y direction is reverse sign
			}
			else
			{
				textScroll = boundaryHeight - lineHeight;
			}

			// Update Text and Renderables
			textRenderable->Translation(Tx, Ty - textScroll, Tz);	// Relative to object space
			// Update Cursor 0
			//cursor0->Translation(Tx + lineWidth, Ty - (int)lineNumber * GraphManager::font->common->lineHeight * fontScale - textScroll, Tz + 0.01);
			// Update Cursor 1
			GetLineInfoAt(cursorPosition1, NULL, &lineNumber1, &lineWidth1);
			cursorLast->Translation(Tx + lineWidth1, Ty - (int)lineNumber1 * GraphManager::font->common->lineHeight * fontScale - textScroll, Tz + 0.01);

			// Update CursorHighlight: Multiline
			// Notice: Neither glm::abs(lineNumber1 - lineNumber0) or glm::abs((int)lineNumber1 - lineNumber0) will work, the former produce 0 when lineNumebr1 is smaller than lineNumber0(due to unknown internal reasons in glm::abs), the latter produce 478983423, something like that due to out of bound
			cursorHighlight->UpdateBuffer_MultiLine((cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1), boundaryWidth - (cursorPosition0 < cursorPosition1 ? lineWidth1 : lineWidth0), boundaryWidth, glm::abs((int)lineNumber1 - (int)lineNumber0) + 1, GraphManager::font->common->lineHeight * fontScale);
			cursorHighlight->Translation(Tx + (cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1), Ty - (int)(cursorPosition0 < cursorPosition1 ? lineNumber0 : lineNumber1) * GraphManager::font->common->lineHeight * fontScale - textScroll, Tz + 0.01);	// Take position of the cursor that is nearest the beginning
		}
		else
		{
			if (bSingleLine)
			{
				// Update Cursor 0
				//cursor0->Translation(Tx + lineWidth + textScroll, Ty - (int)lineNumber * GraphManager::font->common->lineHeight * fontScale, Tz + 0.01);
				// Update Cursor 1
				GetLineInfoAt(cursorPosition1, NULL, &lineNumber1, &lineWidth1);
				cursorLast->Translation(Tx + lineWidth1 + textScroll, Ty - (int)lineNumber1 * GraphManager::font->common->lineHeight * fontScale, Tz + 0.01);

				// Update CursorHighlight: SingleLine
				cursorHighlight->UpdateBuffer_SingleLine(glm::abs(lineWidth1 - lineWidth0), GraphManager::font->common->lineHeight * fontScale);
				cursorHighlight->Translation(Tx + (cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1) + textScroll, Ty - (int)(cursorPosition0 < cursorPosition1 ? lineNumber0 : lineNumber1)* GraphManager::font->common->lineHeight * fontScale, Tz + 0.01);	// Take position of the cursor that is nearest the beginning
			}
			else
			{
				// Update Cursor 0
				//cursor0->Translation(Tx + lineWidth, Ty - (int)lineNumber * GraphManager::font->common->lineHeight * fontScale - textScroll, Tz + 0.01);
				// Update Cursor 1
				GetLineInfoAt(cursorPosition1, NULL, &lineNumber1, &lineWidth1);
				cursorLast->Translation(Tx + lineWidth1, Ty - (int)lineNumber1 * GraphManager::font->common->lineHeight * fontScale - textScroll, Tz + 0.01);

				// Update CursorHighlight: MultiLine
				cursorHighlight->UpdateBuffer_MultiLine((cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1), boundaryWidth - (cursorPosition0 < cursorPosition1 ? lineWidth1 : lineWidth0), boundaryWidth, glm::abs((int)lineNumber1 - (int)lineNumber0) + 1, GraphManager::font->common->lineHeight * fontScale);
				cursorHighlight->Translation(Tx + (cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1), Ty - (int)(cursorPosition0 < cursorPosition1 ? lineNumber0 : lineNumber1) * GraphManager::font->common->lineHeight * fontScale - textScroll, Tz + 0.01);	// Take position of the cursor that is nearest the beginning


				// Debug
				//cout << (cursorPosition0 < cursorPosition1 ? lineWidth0 : lineWidth1) << ":-:" << boundaryWidth - (cursorPosition0 < cursorPosition1 ? lineWidth1 : lineWidth0) << ":-:" << glm::abs(lineNumber1 - lineNumber0) + 1 << endl;
				//cout << "LineNumber 0: " << lineNumber0 << ", LineNumber1: " << lineNumber1 << endl;
			}
			
		}
	}
}

void TextContainer::UpdateBoundary(float areaWidth, float areaHeight, bool fixWidth, bool fixHeight)
{
	boundaryWidth = areaWidth;
	boundaryHeight = areaHeight;

	bFixHeight = fixHeight;
	bFixWidth = fixWidth;

	UpdateRenderables();
}

void TextContainer::SpecifyCursorRender(bool bRender)
{
	bRenderCursor = bRender;
}

void TextContainer::Render()
{
	// Render Stencil Frame
	{
		// __Pending__ Debug using this to see our width and height assignment
		// Diable Colors using masks
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		// Disable Depth Test
		glDisable(GL_DEPTH_TEST);

		// Enable Stencil Buffer
		glEnable(GL_STENCIL_TEST);
		// CLear stencil Buffer
		static int stencilDefualt = 0;
		glClearBufferiv(GL_STENCIL, 0, &stencilDefualt);

		// Setup stencil state
		glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 1, 0xff);
		glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_ZERO, GL_REPLACE);

		// Render the stencil area
		stencilMask->Render();

		// Setup stencil testing for rendering
		glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_EQUAL, 1, 0xff);
		glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

		// Enable Color Drawing
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glEnable(GL_DEPTH_TEST);
	}

	// Render Texts
	textRenderable->Render();

	// Render Cursor
	if (bRenderCursor)
	{
		//cursor0->Render();
		cursorHighlight->Render();
		cursorLast->Render();
	}

	// Disable Stencil Buffer
	glDisable(GL_STENCIL_TEST);
}

// Common Interactions
void TextContainer::OnMouseOver(double xpos, double ypos)
{
	if (bLMBDown)
	{
		// __Debug__: Performance Hazard
		// Lock Second Cursor Numder
		cursorPosition1 = GetNumChars(xpos, ypos);
		UpdateCursors();
	}
	else if (bRMBDown)	// Quick Text Scroll
	{

	}
}

void TextContainer::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// Get Primary Cursor Location
		cursorPosition1 = GetNumChars(xpos, ypos);
		if (!(mods & GLFW_MOD_SHIFT))
		{
			cursorPosition0 = cursorPosition1;
		}
		UpdateCursors();

		bRenderCursor = true;
		bLMBDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		// Lock Second Cursor Numder
		cursorPosition1 = GetNumChars(xpos, ypos);
		UpdateCursors();

		bRenderCursor = true;
		bLMBDown = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		bRMBDown = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		bRMBDown = false;
	}
}

void TextContainer::OnKeyboardButton(int key, int scancode, int action, int mods)
{
	// Avoid control characters when typing using IME
	// Tab is potentially collision with in-IME tab,but assume very rarely people want to do tab in an IME, although Sougou can do that
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS && bPrev == false)
	{
		AddCharacters((unsigned short*)L"    ", 4);	// Add four tabs
	}
	// Backspace
	else if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)
	{
		DeleteCharactersBackward();
	}
	// Delete button
	else if (key == GLFW_KEY_DELETE && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)
	{
		DeleteCharactersForward();
	}
	// Newline
	else if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)	// Also consider KeyPad Enter
	{
		AddCharacters((unsigned short*)L"\n", 1);
	}
	// Add navigation, also caution Conflict with IME
	// Up
	else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)
	{
		MoveCursor(Cursor_Up, mods & GLFW_MOD_SHIFT);
	}
	// Down
	else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)
	{
		MoveCursor(Cursor_Down, mods & GLFW_MOD_SHIFT);
	}
	// Left
	else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)
	{
		MoveCursor(Cursor_Backward, mods & GLFW_MOD_SHIFT);
	}
	// Right
	else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT) && bPrev == false)
	{
		MoveCursor(Cursor_Forward, mods * GLFW_MOD_SHIFT);
	}
	// HOME
	else if (key == GLFW_KEY_HOME && (action == GLFW_PRESS) && bPrev == false)
	{
		MoveCursor(Cursor_Home, mods & GLFW_MOD_SHIFT);
	}
	// END
	else if (key == GLFW_KEY_END && (action == GLFW_PRESS) && bPrev == false)
	{
		MoveCursor(Cursor_End, mods & GLFW_MOD_SHIFT);
	}
	// Select All
	else if (key == GLFW_KEY_A && (action == GLFW_PRESS) && (mods & GLFW_MOD_CONTROL)  && bPrev == false)
	{
		MoveCursor(Cursor_SelectAll);
	}
	// If we received character(i.e. not a shortcut or control character) here, then set bPrev true
	// Of course we have no way to know whether or not OnChar() will be called next, but not our concern.
	else if (mods == 0 && action == GLFW_PRESS)
	{
		bPrev = true;
		// mods == 0 is used to avoid when turning on IME, otherwise after immediate IME switch one will not be able to backspcae etc because bPrev is true
	}
	// The only case left is: when someone tried to enter something while he deleted all that in the IME and then he will discover he cannot backspcae until he enter something successfully. 
	// But in practice this is rarely the case.

	// Paste From Clipboard
	else if (key == GLFW_KEY_V && (action == GLFW_PRESS) && (mods & GLFW_MOD_CONTROL) && bPrev == false)
	{
		PasteFromClipboard();
	}
	// Copy From Clipboard
	else if (key == GLFW_KEY_C && (action == GLFW_PRESS) && (mods & GLFW_MOD_CONTROL) && bPrev == false)
	{
		CopyTextToClipboard();
	}
}

void TextContainer::OnTextInput(unsigned short* text, unsigned int numChars)
{
	bPrev = false;

	AddCharacters(text, numChars);

	// __Debug__ Might Want Update Width/Height here according to Potential User Text Editor Type: Flexible width and height or Fixed width and height
}

void TextContainer::OnDropWindow(int count, const char** paths)
{
	// Update Data
	for (int i = 0; i < count; i++)
	{
		// Currently We are just adding it to text area
		std::wstring utf16 = SystemFunctions::ConvertU8toU16(paths[i]);
		AddCharacters((unsigned short*)utf16.c_str(), utf16.length());
	}
}

void TextContainer::MoveCursor(CursorMovement movement, bool bShiftMod)
{
	// May not exist if we are in reference mode
	if (!textData)
	{
		return;
	}

	switch (movement)
	{
	case Cursor_Forward:
		if (cursorPosition1 + 1 <= textData->GetStringNumChars())
		{
			cursorPosition1 += 1;	// Magic Number to avoid precise calculation
			// Sync Primary Cursor if not Shift isn't pressed
			if (!bShiftMod)
			{
				cursorPosition0 = cursorPosition1;
			}
			UpdateCursors();
		}	
		break;
	case Cursor_Backward:
		if (/*cursorPosition1 - 1 >= 0 && */cursorPosition1 != 0)
		{
			cursorPosition1 -= 1;	// Magic Number to avoid precise calculation
			// Sync Primary Cursor
			if (!bShiftMod)
			{
				cursorPosition0 = cursorPosition1;
			}
			UpdateCursors();
		}
		break;
	case Cursor_Up:
		MoveCursorUp(bShiftMod);
		UpdateCursors();
		break;
	case Cursor_Down:
		MoveCursorDown(bShiftMod);
		UpdateCursors();
		break;
	case Cursor_Home:
		cursorPosition1 = 0;
		if (!bShiftMod)
		{
			cursorPosition0 = cursorPosition1;
		}
		UpdateCursors();
		break;
	case Cursor_End:
		cursorPosition1 = textData->GetStringNumChars();
		if (!bShiftMod)
		{
			cursorPosition0 = cursorPosition1;
		}
		UpdateCursors();
		break;
	case Cursor_SelectAll:
		cursorPosition0 = 0;
		cursorPosition1 = textData->GetStringNumChars();
		UpdateCursors();
		break;
	default:
		break;
	}
}

void TextContainer::PasteFromClipboard()
{
	std::wstring utf16 = SystemFunctions::ConvertU8toU16(GraphManager::GetClipboardString());

	AddCharacters((unsigned short*)utf16.c_str(), utf16.length());
}

void TextContainer::Associate(StringData* newString)
{
	if (bUsingReferencedData)
	{
		cursorPosition0 = cursorPosition1 = 0;	// This line is crucial for correct display
		textData = newString;
		UpdateRenderables();
	}
	else
	{
		// We must be using reference data
		__debugbreak();
	}
}

void TextContainer::CopyTextToClipboard()
{
	int originalStringNumChars = abs((int)(cursorPosition1 - cursorPosition0));
	wchar_t* originalString = (wchar_t*)textData->GetString() + Mini(cursorPosition1, cursorPosition0);
	char* UTF8tring = NULL;
	int requiredUTF8Size = SystemFunctions::U16ToU8((unsigned short*)originalString, originalStringNumChars, NULL, 0);

	UTF8tring = (char*)malloc(requiredUTF8Size + 2);	// UTF8 use two bytes for NULL
	SystemFunctions::U16ToU8((unsigned short*)originalString, originalStringNumChars, UTF8tring, requiredUTF8Size);
	UTF8tring[requiredUTF8Size] = 0;
	UTF8tring[requiredUTF8Size+1] = 0;

	GraphManager::SetClipboardString(UTF8tring);
	free(UTF8tring);
}

void TextContainer::MoveCursorUp(bool bShiftMod)
{
	if (bSingleLine)
	{
		return;
	}

	// Cases:
	// 1. Current Line is the first and last line: in this case: do nothing
	// 2. Current Line is the last/middle line, and a previous line is shorter than current line: in this case go to the end of last line
	// 3. Current Line is the last/middle line, and a previous line has a location furthuer than current cursor: in this case, go to that location
	// Get Current Cursor Position Line Width
	unsigned int currentLine, currentLineChar;
	float currentLineWidth;
	GetLineInfoAt(cursorPosition1, &currentLineChar, &currentLine, &currentLineWidth);

	if (currentLine == 0)// if we have only one line
	{
		// Do nothing
		// If we want to go to beginning of line, we should use HOME
	}
	else
	{
		// Find Neareast Location on an above line
		float lineWidth = 0;
		unsigned int lineNumber = 0;

		bool bFound = false;
		for (unsigned int i = 0; i < cursorPosition1 - currentLineChar - 1; i++)	// i.e. not include current cursor position; -1 for a new line character
		{
			unsigned short character = textData->GetString()[i];
			if (character == (unsigned int)L'\n')
			{
				lineWidth = 0;
				lineNumber++;
			}
			else
			{
				FontChar* charInfo = GraphManager::font->GetChar(character);
				lineWidth += charInfo? (charInfo->xadvance * fontScale) : 0;
				if (lineWidth > boundaryWidth)
				{
					lineWidth = charInfo? (charInfo->xadvance * fontScale) : 0;	//Effectively begin on the next line
					lineNumber++;
				}
			}

			// Check Cursor Jump
			if (currentLine - lineNumber == 1 && lineWidth >= currentLineWidth)
			{
				cursorPosition1 = i + 1;	// i is the location, i+1 is the number of characters
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			cursorPosition1 = cursorPosition1 - currentLineChar - 1;	// Use the last character of the last line
		}

		// Sync Primary Cursor
		if(!bShiftMod)cursorPosition0 = cursorPosition1;
	}
}
void TextContainer::MoveCursorDown(bool bShiftMod)
{
	if (bSingleLine)
	{
		return;
	}

	// Cases:
	// 1. Current Line is the last line: in this case: do nothing
	// 2. Current Line is the first/middle line, and a succeeding line is shorter than current line: in this case go to the end of succeeding line
	// 3. Current Line is the first/middle line, and a succeeding line has a location furthuer than current cursor: in this case, go to that location - 1
	// Get Current Cursor Position Line Width
	unsigned int currentLine;
	float currentLineWidth;
	GetLineInfoAt(cursorPosition1, &currentLine, NULL, &currentLineWidth);

	unsigned int lastLine;
	GetLineInfoAt(textData->GetStringNumChars(), NULL, &lastLine);
	if (currentLine == lastLine) // If we are the last line
	{
		// Do nothing
		// If we want to go to end of line, we should use END
		// return;	// According to our logic in the Search loop, the cursor will jump to the end of the string, unless we return here. But I found that quite expected, so I will leave it there.
	}
	else
	{
		// Find Neareast Location on a succeeding line
		float lineWidth = currentLineWidth;
		unsigned int lineNumber = currentLine;

		// Begin with next character, which is at location cursorNumCharacters, because current cursor is acutually at location cursorNumCharacters -1
		for (unsigned int i = cursorPosition1; i < textData->GetStringNumChars(); i++)
		{
			unsigned short character = textData->GetString()[i];
			if (character == (unsigned int)L'\n')
			{
				lineWidth = 0;
				lineNumber++;
			}
			else
			{
				FontChar* charInfo = GraphManager::font->GetChar(character);
				lineWidth += charInfo? (charInfo->xadvance * fontScale) : 0;
				if (lineWidth > boundaryWidth)
				{
					lineWidth = charInfo ? (charInfo->xadvance * fontScale) : 0;	//Effectively begin on the next line
					lineNumber++;
				}
			}

			// Check Cursor Jump
			if (lineNumber - currentLine == 1 && lineWidth >= currentLineWidth)
			{
				cursorPosition1 = i + 1;
				break;
			}
			if (lineNumber - currentLine == 2)
			{
				// In first occurance of line number difference becoming 2, we know we haven't found what we want
				cursorPosition1 = i;	// Use the last character, i+1 is the position of a newline
				break;	// A break is very important here....
			}
			if (i == textData->GetStringNumChars() - 1)
			{
				// If we hit the end of the string, we know we haven't found what we want
				cursorPosition1 = textData->GetStringNumChars();
				break;	// A break is NOT very important here....
			}
		}

		// Sync Primary Cursor
		if(!bShiftMod)cursorPosition0 = cursorPosition1;
	}
}