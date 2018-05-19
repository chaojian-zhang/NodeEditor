#include "RenderableLine.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include <math.h>

RenderableLine::RenderableLine(Interface* parent, float width, float height, float color[3], bool d)
	:Renderable(Programs::ShapeProgram, parent), bDynamic(d)
{
	// Save Data
	memcpy(lineColor, color, sizeof(float) * 3);

	UpdateBuffer(width, height);
}

void RenderableLine::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	timeScale = glm::mod((float)GraphManager::systemTime, 1.f) > 0.5 ? 1.f : 0.f;
	// Set uniforms
	glUniform1f(3, timeScale);	// time scale

	// Draw Face
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	glUniform3fv(2, 1, lineColor);	// color
	// Render the quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RenderableLine::UpdateBuffer(float width, float height)
{
	// Prepare Data, CW
	float vertexData[4 * 3] =
	{
		0, 0, 0.f,
		width, 0, 0.f,
		width, -height, 0.f,
		0, -height, 0.f
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertexData, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	// Might by dynamic
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
	glEnableVertexAttribArray(0);
}
void RenderableLine::UpdateProperty(float* color)
{
	memcpy(lineColor, color, sizeof(float) * 3);
}