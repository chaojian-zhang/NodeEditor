#include "RenderableString.h"
#include "GraphManager.h"
#include "Shaders.h"
#include "MathElegent.h"
#include "Interface.h"
#include "Constants.h"

/* Major Accomodation for 3D:
1. Update Buffer use 1/1024 scale
2. All unsigned int/ int to flaot
3. Render() Matrix Feeding
*/

RenderableString::RenderableString(Interface* parent, unsigned short* content, unsigned int numChars, float * inColor, unsigned int fontSize, bool dynamic, bool D3)
	:bDynamic(dynamic), Renderable(Programs::TextProgram, parent), b3D(D3)
{
	// Generate OpenGL Objects
	glGenBuffers(2, buffers);

	// Save Data
	memcpy(color, inColor, sizeof(float) * 3);

	UpdateBuffers_Line(content, numChars, fontSize);
}

RenderableString::RenderableString(Interface* parent, unsigned short* content, unsigned int numChars, float * inColor, float fieldWidth, unsigned int fontSize, bool dynamic, bool D3)
	: bDynamic(dynamic), Renderable(Programs::TextProgram, parent), renderNumCharacters(numChars), b3D(D3)
{
	// Generate OpenGL Objects
	glGenBuffers(2, buffers);

	// Save Data
	memcpy(color, inColor, sizeof(float) * 3);

	UpdateBuffers_Area(content, numChars, fieldWidth, fontSize);
}

RenderableString::~RenderableString()
{
	// delete openGL objects
	glDeleteBuffers(2, buffers);
}

void RenderableString::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	GraphManager::UseNewTexture0(GraphManager::font->GetCharMap(0));
	GraphManager::UseNewTexture1(GraphManager::font->GetCharMap(1));

	// Set uniform
	glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	glUniform3fv(4, 1, color);	// color
	glUniform1f(5, transparancy);

	// Render the image quad
	glDrawArrays(GL_TRIANGLES, 0, renderNumCharacters * 6);
}

void RenderableString::UpdateBuffers_Line(unsigned short* content, unsigned int numChars, unsigned int fontSize)
{
	renderNumCharacters = numChars;

	// Allocate Temp Date for the string
	Vector3f* vertices = new Vector3f[numChars * 6];	// Each Char will use a quad
	float* vertCoords = new float[numChars * 6 * 2];	// Each Char has 6 coords, each has two components
	char* mapIndexes = new char[numChars * 6];

	// Apply a fontSize Scaling to the vertice
	float fontScale = (float)fontSize / GraphManager::font->info->fontSize * (b3D ? InterfaceConstant::PixelToMeterScaleFactor : 1);
	// Variables
	FontCommon* common = GraphManager::font->common;
	float drawPenX = 0;
	float drawPenY = -common->base * fontScale;

	// Update all characters
	for (int i = 0; i < numChars; i++)
	{
		// Handle Newline Character: Although by Design when using labels newline character actually should not appear. That is, if a user type a new line in a non-formated text field(i.e. text box), then we should not even add to stringData
		if (content[i] == (unsigned int)L'\n')
		{
			// Do nothing with Pen Position
			// drawPenY = drawPenY;
			// drawPenX = drawPenX;

			// Enter, eventhough not a character, still need to be rendered, so it is necessary to give it a mesh shape
			vertices[i * 6 + 0]
				= vertices[i * 6 + 3]
				= vertices[i * 6 + 1]
				= vertices[i * 6 + 2]
				= vertices[i * 6 + 4]
				= vertices[i * 6 + 5]
				= Vector3f{ 0, 0, 0 };
		}
		// Handle Tab Character
		else if (content[i] == (unsigned int)L'\t' || content[i] < 32)
		{
			// Do nothing with Pen Position
			// drawPenY = drawPenY;
			// drawPenX = drawPenX;

			vertices[i * 6 + 0]
				= vertices[i * 6 + 3]
				= vertices[i * 6 + 1]
				= vertices[i * 6 + 2]
				= vertices[i * 6 + 4]
				= vertices[i * 6 + 5]
				= Vector3f{ 0, 0, 0 };
		}
		else
		{
			// Quick Access
			FontChar* character = GraphManager::font->GetChar(content[i]);
			int mapWidth = common->scaleW;
			int mapHeight = common->scaleH;

			// CW begin with right triangle on UL corner
			// Working in openGL coordinates: that is the second quadrant with x > 0 and y < 0
			float xoffset = character->xoffset * fontScale;
			float yoffset = character->yoffset * fontScale;
			float width = character->width * fontScale;
			float height = character->height * fontScale;
			float base = common->base * fontScale;
			vertices[i * 6 + 0]
				= vertices[i * 6 + 3]
				= Vector3f{ drawPenX + xoffset, drawPenY + base - yoffset, 0.f };
			vertices[i * 6 + 1]
				= Vector3f{ drawPenX + xoffset + width, drawPenY + base - yoffset, 0.f };
			vertices[i * 6 + 2]
				= vertices[i * 6 + 4]
				= Vector3f{ drawPenX + xoffset + width, drawPenY + base - yoffset - height, 0.f };
			vertices[i * 6 + 5]
				= Vector3f{ drawPenX + xoffset, drawPenY + base - yoffset - height, 0.f };

			// Update Vert Coords
			// X Coordinate
			vertCoords[i * 12 + 0]	// tc01
				= vertCoords[i * 12 + 6]	// tc11
				= vertCoords[i * 12 + 10]	// tc13
				= (float)(character->x) / mapWidth;
			vertCoords[i * 12 + 2]		// tc02
				= vertCoords[i * 12 + 4]	// tc03
				= vertCoords[i * 12 + 8]	// tc12
				= (float)(character->x + character->width) / mapWidth;
			// Y Coordinates
			vertCoords[i * 12 + 1]	// tc01
				= vertCoords[i * 12 + 3]	// tc02
				= vertCoords[i * 12 + 7]	// tx11
				= (float)(character->y) / mapHeight;
			vertCoords[i * 12 + 5]	// tc03
				= vertCoords[i * 12 + 9]	// tc12
				= vertCoords[i * 12 + 11]	// tc13
				= (float)(character->y + character->height) / mapHeight;

			// Update Pen Position for next drawing character
			// Horizonatal Advance Only, no need to do vertical advance since no formatting is involved
			drawPenX += character->xadvance * fontScale;

			// Update IDs, although a great waste of space, but should be ok
			mapIndexes[i * 6 + 0] = character->page;
			mapIndexes[i * 6 + 1] = character->page;
			mapIndexes[i * 6 + 2] = character->page;
			mapIndexes[i * 6 + 3] = character->page;
			mapIndexes[i * 6 + 4] = character->page;
			mapIndexes[i * 6 + 5] = character->page;
		}
	}

	// Update String DImensions
	stringWidth = abs(drawPenX);
	stringHeight = abs(drawPenY - (common->lineHeight - common->base)* fontScale);

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, numChars * 6 * 3 * sizeof(float), vertices, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	// Pending Bench Mark, since the data store is changing so we have to reallocate every time
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhere to TextProgram Layout
	glEnableVertexAttribArray(0);

	// Generate TextCoord Buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, numChars * 6 * 2 * sizeof(float), vertCoords, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Generate mapID Buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, numChars * 6 * sizeof(char), mapIndexes, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_BYTE, 0, 0);
	glEnableVertexAttribArray(2);

	// Delete allocates spcae
	delete[] vertices;
	delete[] vertCoords;
	delete[] mapIndexes;
}

// __Debug__: Pending Profiling and Set Up Discard Buffer openGL command, which might speed things up a bit
void RenderableString::UpdateBuffers_Area(unsigned short* textContent, unsigned int numChars, float fieldWidth, unsigned int fontSize)
{
	renderNumCharacters = numChars;

	// Allocate Temp Date for the string
	//Vector3f* vertices = new Vector3f[numChars * 6];	// Each Char will use a quad
	Vector3f* vertices = NULL;
	try {
		vertices = new Vector3f[numChars * 6];	// Debug using Exception mechanism in order to log error information
	}
	catch (const std::bad_alloc& e) {
		std::cout << "Allocation failed: " << e.what() << '\n';
	}
	float* vertCoords = new float[numChars * 6 * 2];	// Each Char has 6 coords, each has two components
	char* mapIndexes = new char[numChars * 6];

	// Apply a fontSize Scaling to the vertice
	float fontScale = (float)fontSize / GraphManager::font->info->fontSize * (b3D ? InterfaceConstant::PixelToMeterScaleFactor : 1);	// Alright, I agree the type of fontSize and that of info->fontSize are not consistent, but this happpen for a reason: because we are BMFont specifies so
	// Variables
	FontCommon* common = GraphManager::font->common;
	float drawPenX = 0;
	float drawPenY = -common->base * fontScale;

	// Update all characters
	for (int i = 0; i < numChars; i++)
	{
		// Handle Newline Character
		if (textContent[i] == (unsigned int)L'\n')
		{
			// Update Pen Position
			drawPenY -= common->lineHeight * fontScale;
			drawPenX = 0;

			// Enter, eventhough not a character, still need to be rendered, so it is necessary to give it a mesh shape
			vertices[i * 6 + 0]
				= vertices[i * 6 + 3]
				= vertices[i * 6 + 1]
				= vertices[i * 6 + 2]
				= vertices[i * 6 + 4]
				= vertices[i * 6 + 5]
				= Vector3f{ 0, 0, 0 };
		}
		// Handle Tab Character and all other unrecognized characters
		else if (textContent[i] == (unsigned int)L'\t' || textContent[i] < 32)
		{
			//// Update Pen Position
			//drawPenY -= common->lineHeight * fontScale;
			//drawPenX = 0;

			// Ignore, this normally shoudn't appear
			vertices[i * 6 + 0]
				= vertices[i * 6 + 3]
				= vertices[i * 6 + 1]
				= vertices[i * 6 + 2]
				= vertices[i * 6 + 4]
				= vertices[i * 6 + 5]
				= Vector3f{ 0, 0, 0 };
		}
		else
		{
			// Quick Access
			FontChar* character = GraphManager::font->GetChar(textContent[i]);
			int mapWidth = common->scaleW;
			int mapHeight = common->scaleH;

			// Draw Position
			Vector3f pen;	// Temperaory pen
			// Reevaluate draw position depending on the width of the pending drawn character
			if (drawPenX + character->xadvance * fontScale > fieldWidth)
			{
				pen = { 0, drawPenY - common->lineHeight * fontScale, 0 };
			}
			else
			{
				pen = { drawPenX, drawPenY, 0 };
			}

			// CW begin with right triangle on UL corner
			// Working in openGL coordinates: that is the second quadrant with x > 0 and y < 0
			float xoffset = character->xoffset * fontScale;
			float yoffset = character->yoffset * fontScale;
			float width = character->width * fontScale;
			float height = character->height * fontScale;
			float base = common->base * fontScale;
			vertices[i * 6 + 0]
				= vertices[i * 6 + 3]
				= Vector3f{ pen.x + xoffset, pen.y + base - yoffset, 0.f };	// For Renderables we do not offset anything, it is Interface Element or CanvasNode's responsibility if they want to offset us
			vertices[i * 6 + 1]
				= Vector3f{ pen.x + xoffset + width, pen.y + base - yoffset, 0.f };
			vertices[i * 6 + 2]
				= vertices[i * 6 + 4]
				= Vector3f{ pen.x + xoffset + width, pen.y + base - yoffset - height, 0.f };
			vertices[i * 6 + 5]
				= Vector3f{ pen.x + xoffset, pen.y + base - yoffset - height, 0.f };

			// Update Vert Coords
			// X Coordinate
			vertCoords[i * 12 + 0]	// tc01
				= vertCoords[i * 12 + 6]	// tc11
				= vertCoords[i * 12 + 10]	// tc13
				= (float)(character->x) / mapWidth;
			vertCoords[i * 12 + 2]		// tc02
				= vertCoords[i * 12 + 4]	// tc03
				= vertCoords[i * 12 + 8]	// tc12
				= (float)(character->x + character->width) / mapWidth;
			// Y Coordinates
			vertCoords[i * 12 + 1]	// tc01
				= vertCoords[i * 12 + 3]	// tc02
				= vertCoords[i * 12 + 7]	// tx11
				= (float)(character->y) / mapHeight;
			vertCoords[i * 12 + 5]	// tc03
				= vertCoords[i * 12 + 9]	// tc12
				= vertCoords[i * 12 + 11]	// tc13
				= (float)(character->y + character->height) / mapHeight;

			// Update Pen Position for next drawing character
			// Horizonatal and Vertical Advance
			drawPenX += character->xadvance * fontScale;
			if (drawPenX > fieldWidth)
			{
				drawPenX = character->xadvance * fontScale;	// effectively begin at new line
				drawPenY -= common->lineHeight *fontScale;
			}

			// Update IDs, although a great waste of space, but should be ok
			mapIndexes[i * 6 + 0] = character->page;
			mapIndexes[i * 6 + 1] = character->page;
			mapIndexes[i * 6 + 2] = character->page;
			mapIndexes[i * 6 + 3] = character->page;
			mapIndexes[i * 6 + 4] = character->page;
			mapIndexes[i * 6 + 5] = character->page;
		}
	}

	// Update String DImensions
	stringWidth = fieldWidth;
	stringHeight = abs(drawPenY - (common->lineHeight - common->base)* fontScale);

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, numChars * 6 * 3 * sizeof(float), vertices, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	// Pending Bench Mark, since the data store is changing so we have to reallocate every time
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhere to TextProgram Layout
	glEnableVertexAttribArray(0);

	// Generate TextCoord Buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, numChars * 6 * 2 * sizeof(float), vertCoords, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Generate mapID Buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, numChars * 6 * sizeof(char), mapIndexes, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_BYTE, 0, 0);
	glEnableVertexAttribArray(2);

	// Delete allocates spcae
	delete[] vertices;
	delete[] vertCoords;
	delete[] mapIndexes;
}

void RenderableString::UpdateProperty(float* inColor)
{
	memcpy(color, inColor, sizeof(float) * 3);
}

void RenderableString::UpdateTransparancy(float value)
{
	transparancy = value;
}

float RenderableString::GetStringWidth()
{
	return stringWidth;
}

float RenderableString::GetStringHeight()
{
	return stringHeight;
}