#include "RenderableDebugRay.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include <math.h>
#include "Interface.h"

// Changelist: All int to float; 20 vertices to 23 vertices

float RenderableDebugRay::color[3] = {1,0,0};

RenderableDebugRay::RenderableDebugRay(glm::vec3 origin, glm::vec3 direction, float length)
	:Renderable(Programs::ShapeProgram, GraphManager::canvas)
{
	UpdateBuffer(origin, direction, length);
}

// Might be too many draw calls, might want to condense
void RenderableDebugRay::Render()
{
	// Debug
	glLineWidth(3);

	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);

	glUniform1f(3, 1);	// time scale
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(parentInterface->GetProjectMatrix() * parentInterface->GetViewMatrix()/*Canvas::GetCamera()->project * Canvas::GetCamera()->view*/));	// Model In World Space already, no need to multiply Model Matrix; Also Multiply by view matri is neccesary if want to move with view
	glUniform3fv(2, 1, color);	// color

	glDrawArrays(GL_LINES, 0, 2);
}

void RenderableDebugRay::UpdateBuffer(glm::vec3 origin, glm::vec3 direction, float length)
{
	float vertexData[2 * 3] =
	{
		origin.x, origin.y, origin.z - 0.05,	// Avoid Clipping
		origin.x + direction.x * length, origin.y + direction.y * length, origin.z + direction.z * length	// Do not add 0.05 here
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), vertexData, GL_DYNAMIC_DRAW );	// Dynamic
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
	glEnableVertexAttribArray(0);
}

void RenderableDebugRay::UpdateBuffer_Line(glm::vec3 origin, glm::vec3 endPoint)
{
	float vertexData[2 * 3] =
	{
		origin.x, origin.y, origin.z, 
		endPoint.x, endPoint.y, endPoint.z
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), vertexData, GL_DYNAMIC_DRAW);	// Dynamic
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
	glEnableVertexAttribArray(0);
}