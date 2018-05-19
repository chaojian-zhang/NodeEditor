#include "RenderableRoundSidesQuad.h"
#include "GraphManager.h"
#include <math.h>
#include "Interface.h"
#include "Constants.h"

RenderableRoundSidesQuad::RenderableRoundSidesQuad(Interface* parent, float width, float height, bool border, float* bcolor, float* acolor, bool dynamic, bool d3)
	:Renderable(Programs::ShapeProgram, parent), bBorder(border), bDynamic(dynamic)
{
	// Save Data
	memcpy(borderColor, bcolor, sizeof(float) * 3);
	memcpy(areaColor, acolor, sizeof(float) * 3);

	UpdateBuffer(width, height);

	// Resize a bit
	float borderSize = d3 ? (0.005) : (InterfaceConstant::DefaultPixelBorderSize);
	float scaleX = (float)(borderSize * 2 + width) / width;	// Seek a fine control
	float scaleY = (float)(borderSize * 2 + height) / height;
	modelResize = glm::scale(glm::mat4(1), glm::vec3(scaleX, scaleY, 1));	// Resize
	// modelResize = glm::translate(modelResize, glm::vec3(-(int)width * (scaleX - 1) / 2, height * (scaleY - 1) / 2, -.1f));	// Offset a bit + offset back into Z
	modelResize = glm::translate(modelResize, glm::vec3(-borderSize, borderSize, -.001f));	// Offset a bit + offset back into Z
}

// Might be too many draw calls, might want to condense
void RenderableRoundSidesQuad::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	// Set common uniforms
	glUniform1f(3, 1);	// time scale

	// Draw Border First
	if (bBorder)
	{
		// Bind Data, parameters already using openGL coordinates
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world*modelResize));	// MVP; notice we resize first them translate
		glUniform3fv(2, 1, borderColor);	// color
		// Render the image quad
		glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
	}

	// Draw Face
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	glUniform3fv(2, 1, areaColor);	// color
	// Render the image quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
}

void RenderableRoundSidesQuad::UpdateBuffer(float width, float height)
{
	// Essentially the same as Renderable2DRoundCornerQuad, except that we are semi circle in this case
	float smallestHalfDimension = Minf(width / 2, height / 2);
	float radius = smallestHalfDimension;

	// Prepare Data, CW
	// We divide each corner into 4 sections: 0d, 22.5d, 45d, 67.5d, 90d
	// Use a drawing to help visualize vertices
	float	dsR[6] =		// stand for deltaS, in reverse direction; we use int here to save later do explicit conversion for vertexData
	{
		// Radius should be positive, so all the value calculated are still positive
		radius - radius* cos(0.392),// d22.5dx
		radius - radius* sin(0.392),// d22.5dy
		radius - radius* cos(0.785),// d45dx
		radius - radius* sin(0.785),// d45dy
		radius - radius* cos(1.178),// d67.5dx
		radius - radius* sin(1.178),// d67.5dy
	};
	float vertexData[20 * 3] =
	{
		// Corner UL
		0, -radius, 0,
		dsR[0], -dsR[1], 0,
		dsR[2], -dsR[3], 0,
		dsR[4], -dsR[5], 0,
		radius, 0, 0,
		// Corner UR
		width - radius, 0, 0,
		width - dsR[4], -dsR[5], 0,	// Notice in a reverse order compared with the first(upper-left) corner
		width - dsR[2], -dsR[3], 0,
		width - dsR[0], -dsR[1], 0,
		width, -radius, 0,
		// COner LR
		width, -height + radius, 0,	// Remember in openGL second quadrant
		width - dsR[0], -height + dsR[1], 0,
		width - dsR[2], -height + dsR[3], 0,
		width - dsR[4], -height + dsR[5], 0,
		width - radius, -height, 0,
		// Connoer LL
		radius, -height, 0,
		dsR[4], -height + dsR[5], 0,
		dsR[2], -height + dsR[3], 0,
		dsR[0], -height + dsR[1], 0,
		0, -height + radius, 0
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 20 * 3 * sizeof(float), vertexData, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	// Might by dynamic
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
	glEnableVertexAttribArray(0);
}
void RenderableRoundSidesQuad::UpdateProperty(bool border, float* bcolor, float* acolor)
{
	memcpy(borderColor, bcolor, sizeof(float) * 3);
	memcpy(areaColor, acolor, sizeof(float) * 3);
	bBorder = border;
}