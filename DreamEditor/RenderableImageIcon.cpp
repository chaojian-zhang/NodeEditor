#include "RenderableImageIcon.h"
#include "GraphManager.h"

RenderableImageIcon::RenderableImageIcon(Interface* parent, float size, float coords[8], GLuint inTexture)
	:texture(inTexture), Renderable(Programs::SimpleTextureProgram, parent)
{
	// Generate OpenGL Object
	glGenBuffers(1, &tBuffer);

	// Prepare Data, CW
	float vertexData[4*3] = 
	{
		0, 0, 0.f,
		size, 0, 0.f,
		size, -size, 0.f,
		0, -size, 0.f
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertexData, GL_STATIC_DRAW);	// Image Icons are always static
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to SimpleTextureProgram
	glEnableVertexAttribArray(0);
	// Feed texture coordinate Data to openGL
	glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), coords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

RenderableImageIcon::~RenderableImageIcon()
{
	// Delete buffer
	glDeleteBuffers(1, &tBuffer);
	// Texture is not managed by us, so do not delete
}

void RenderableImageIcon::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	GraphManager::UseNewTexture0(texture);
	// Bind Data, parameters already using openGL coordinates
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	// Feed in Texture Rotation
	glUniform1f(3, 0.f);
	glUniform1f(4, 1.f);

	// Render the image quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}