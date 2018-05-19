#include "RenderableRoundCornerFrame.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include <math.h>
#include "Interface.h"

// Changes: Int->Float

RenderableRoundCornerFrame::RenderableRoundCornerFrame(float width, float height, float* bcolor, float cornersize, bool d, bool render, float inOpacity)
	:Renderable(Programs::ShapeProgram, GraphManager::canvas), bDynamic(d), bRender(render), opacity(inOpacity)
{
	// Save Data
	memcpy(borderColor, bcolor, sizeof(float) * 3);

	UpdateBuffer(width, height, cornersize);
}

// Might be too many draw calls, might want to condense
void RenderableRoundCornerFrame::Render()
{
	if (bRender)
	{
		// Set up states
		glBindVertexArray(VAO);
		GraphManager::UseNewProgram(program);
		// Set uniforms
		glUniform1f(3, opacity);	// time scale
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
		glUniform3fv(2, 1, borderColor);	// color

		// Render the quad Frame
		glLineWidth(2);	// Debug_Temp: Pending make it a variable
		glDrawArrays(GL_LINE_LOOP, 0, 20);
	}
}

void RenderableRoundCornerFrame::UpdateBuffer(float width, float height, float cornersize)
{
	// Clamp Border Size
	float smallestHalfDimension = Minf(width / 2, height / 2);
	float radius = Minf(cornersize, smallestHalfDimension);

	// Prepare Data, CW
	// We divide each corner into 4 sections: 0d, 22.5d, 45d, 67.5d, 90d
	// Use a drawing to help visualize vertices
	float dsR[6] =		// stand for deltaS, in reverse direction
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
void RenderableRoundCornerFrame::UpdateProperty(float* bcolor, bool render, float inOpacity)
{
	opacity = inOpacity;
	bRender = render;
	memcpy(borderColor, bcolor, sizeof(float) * 3);
}