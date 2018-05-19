#pragma once
#include "StringData.h"
#include "RenderableLine.h"
#include "RenderableCursorHighlight.h"
#include "RenderableString.h"
#include "RenderableRoundCornerQuad.h"
#include <glm\matrix.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class Interface;

enum CursorMovement
{
	Cursor_Forward,
	Cursor_Backward,
	Cursor_Up,
	Cursor_Down,
	Cursor_Home,
	Cursor_End,
	Cursor_SelectAll
};

// __Debug__: Pending PRofiling: How scalable is our full-buffer update method when encountering very long texts, e.g. 65536 characters?
/*TextContainer provides a formated multi-line text processing container with RString, RCursor, and no Decorations;
If specified as single line, then using single line format*/
// A text container WILL create full text renderable, so that it is trivial to scroll long texts; Another choise would be create texts renderabls only to the range of width/height, but that would require update renderables every time we scroll
class TextContainer
{
public:
	// textWidth will be the text area width, which is smaller than the actual upper level container(e.g. interface Elemnt or ndoe object) width since we do not have decorations
	TextContainer(Interface* inParent, unsigned short* initialString, unsigned int numChars,
		unsigned int fSize, float color[3], float areaWidth, float areaHeight, bool singleLine, bool d3, bool dynamic, bool fixWidth, bool fixHeight, StringData* data = NULL, bool useReference = false);	// Receive and Initialize Necessary Parameters; Notice that is the user devides to pass in a referenced data, the privilage to edit that data should be kept only be the textContainer, i.e. the user own the data but should only manipulate it through text container; If input numChars is not 0, this function will override what is originally inside stringData
	// Content can be null, in that case just a reserver area; If areaHeight is non-zero, then we wil clamp rendering
	~TextContainer();

	// Common Interface
	float GetBoundaryWidth();
	float GetBoundaryHeight();
	void Translation(float x, float y, float z = 0);	// Update Renderable Location
	void Rotation(float x, float y, float z = 0);	// Update Renderable Rotation
	void Scaling(float x, float y, float z = 1);
	void Translate(float dx, float dy, float dz = 0);	// Update Renderable Location
	void Rotate(float x, float y, float dz = 0);	// Update Renderable Rotation
	void Scale(float x, float y, float dz = 1);
	void UpdateCursors();	// Update Curosrs Location
	void UpdateBoundary(float areaWidth, float areaHeight, bool fixWidth, bool fixHeight);
	void SpecifyCursorRender(bool bRender);
	void Render();	// Provide Unified Rendering Interface for Texts

	// Common Interaction Interface
	void OnMouseOver(double xpos, double ypos);	// Do a more real time cursor and cursor highlight update
	void OnMouseButton(int button, int action, int mods, double xpos, double ypos);		// Provide Mouse Interaction; relative to origin of TextContainer
	void OnKeyboardButton(int key, int scancode, int action, int mods);
	void OnTextInput(unsigned short* text, unsigned int numChars);
	void OnDropWindow(int count, const char** paths);
	void MoveCursor(CursorMovement movement, bool bShiftMod = false);
	void CopyTextToClipboard();
	void PasteFromClipboard();

	// Associate: Used by TextEditor in PropertyPanel
	void Associate(StringData* newString);

private:
	void MoveCursorUp(bool bShiftMod);
	void MoveCursorDown(bool bShiftMod);

	// Text Processing Interface
	void AddCharacters(unsigned short* string, unsigned int numChars);	// Remember to take cursors into consideration
	void DeleteCharactersBackward();	// Take cursors into consideration
	void DeleteCharactersForward();
	// @Return lineCharNum as num of chars from the beginning of this line
	// @Return lineNumber begin with 0
	// @Return pixel/meter width from current line beginning upto the line character
	// @Usage linePixelHeight might be calculated: begin with 0. i.e. for lineNumber 0, lineHeight is 0, for lineNumber 1, lineHeight = GraphManager::font->common->lineHeight * fontScale
	void GetLineInfoAt(unsigned int numChars, unsigned int* lineCharNum = NULL, unsigned int* lineNum = NULL, float* lineWidth = NULL);
	// @Return numChar at location; 
	// @Inputs in pixel/meter units relative to origin of the TextContainer, that is, y should be negative
	unsigned int GetNumChars(float x, float y);

	void UpdateRenderables();

private:
	static float highlightColor[3];

private:
	// Negative indicate left offset, positive indicates right offset, btw, this value shall never be positive(unless we enable spring like mouse scrolling effect, but NO, we are not gonna do that);
	// For TextField, this would indicate Horizontal Scroll
	// For TextContainer, this would indicate Vertical Scroll
	float textScroll = 0;	// Unit in meter/pixels
	unsigned int cursorPosition0 = 0;	// num chars
	unsigned int cursorPosition1 = 0;
	StringData* textData;
	float Tx, Ty, Tz;	// Our World Space Coordinate
	bool bLMBDown = false;
	bool bRMBDown = false;

private:
	// Parameters
	bool bUsingReferencedData = false;	// Indicate whether the stringData belongs to ourselves
	bool bPrev = false;	// IME collision resolution
	bool bSingleLine;
	bool b3D;	// Allow pixel-to-meter scale
	bool bFixWidth = false, bFixHeight = false;	// If width and height are fixed, then we adhere to the constructor passed in value and never update them ourselves
	float boundaryWidth = 0, boundaryHeight = 0;
	unsigned int fontSize;
	float fontScale;
	float textColor[3];
	Interface* parent;

private:
	// Render only when active
	bool bRenderCursor = false;
	RenderableLine* cursorLast;
	RenderableCursorHighlight* cursorHighlight;

	// Alwasy render
	RenderableString* textRenderable;
	RenderableRoundCornerQuad* stencilMask;
};