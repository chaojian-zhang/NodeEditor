#include "RenderableCursorHighlight.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include <math.h>

RenderableCursorHighlight::RenderableCursorHighlight(Interface* parent, float color[3])
	:Renderable(Programs::ShapeProgram, parent)
{
	// Save Data
	memcpy(highlightColor, color, sizeof(float) * 3);

	// Default Initialization
	UpdateBuffer_SingleLine(0.f, 0.f);
}

void RenderableCursorHighlight::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	// Set uniforms
	glUniform1f(3, transparancy);	// transparency

	// Draw Face
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	glUniform3fv(2, 1, highlightColor);	// color
	// Render the shape
	if (bSingleLine)
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, 6*3);
	}
}

void RenderableCursorHighlight::UpdateBuffer_SingleLine(float spanWidth, float lineHeight)
{
	// Single Line

	// Prepare Data, CW
	float vertexData[4 * 3] =
	{
		0.f, 0.f, 0.f,
		spanWidth, 0.f, 0.f,
		spanWidth, -lineHeight, 0.f,
		0, -lineHeight, 0.f
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertexData, GL_DYNAMIC_DRAW );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
	glEnableVertexAttribArray(0);

	bSingleLine = true;
}

void RenderableCursorHighlight::UpdateBuffer_MultiLine(float forward, float backward, float spanWidth, unsigned int numLines, float lineHeight)
{
	// Single Line case for multiline
	if (numLines == 1)
	{
		// Prepare Data, CW
		float vertexData[4 * 3] =
		{
			0.f, 0.f, 0.f,
			spanWidth - forward - backward, 0.f, 0.f,
			spanWidth - forward - backward, -lineHeight, 0.f,
			0, -lineHeight, 0.f
		};

		// Initialize OpenGL obejcts
		glBindVertexArray(VAO);
		// Generate Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertexData, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
		glEnableVertexAttribArray(0);

		bSingleLine = true;
	}
	else
	{
		// Prepare Data, CW, notice origin is set to be the forward edge
		// Draw the metrics on the paper and you will see how it is organized: forward, backward, boundWidth, lineNumber
		float vertexData[(6 * 3) * 3] =
		{
			// Part1
			0.f, 0.f, 0.f,
			spanWidth - forward, 0.f, 0.f,
			spanWidth - forward, -lineHeight, 0.f,
			0.f, 0.f, 0.f,
			spanWidth - forward, -lineHeight, 0.f,
			0.f, -lineHeight, 0.f,

			// Part2
			-forward, -lineHeight, 0.f,
			spanWidth - forward, -lineHeight, 0.f,
			spanWidth - forward, -lineHeight - lineHeight*(numLines - 2), 0.f,
			-forward, -lineHeight, 0.f,
			spanWidth - forward, -lineHeight - lineHeight*(numLines - 2), 0.f,
			-forward, -lineHeight - lineHeight*(numLines - 2), 0.f,

			// Part3
			-forward, -lineHeight - lineHeight*(numLines - 2), 0.f,
			spanWidth - forward - backward, -lineHeight - lineHeight*(numLines - 2), 0.f,
			spanWidth - forward - backward, -lineHeight * numLines, 0.f,
			-forward, -lineHeight - lineHeight*(numLines - 2), 0.f,
			spanWidth - forward - backward, -lineHeight * numLines, 0.f,
			-forward, -lineHeight * numLines, 0.f
		};

		// Initialize OpenGL obejcts
		glBindVertexArray(VAO);
		// Generate Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, (6 * 3) * 3 * sizeof(float), vertexData, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
		glEnableVertexAttribArray(0);

		bSingleLine = false;
	}
}

void RenderableCursorHighlight::UpdateProperty(float trans, float* color)
{
	if (color)
	{
		memcpy(highlightColor, color, sizeof(float) * 3);
	}

	transparancy = trans;
}