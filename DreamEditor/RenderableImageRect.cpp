#include "RenderableImageRect.h"
#include "GraphManager.h"
#include "Interface.h"

RenderableImageRect::RenderableImageRect(Interface* parent, float width, float height, const float coords[8], GLuint inTexture)
	:texture(inTexture), Renderable(Programs::SimpleTextureProgram, parent)
{
	// Generate OpenGL Object
	glGenBuffers(1, &tBuffer);

	// Prepare Data, CW
	float vertexData[4*3] = 
	{
		0,0, 0,
		width,0,0,
		width,-height,0,
		0,-height,0
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertexData, GL_STATIC_DRAW);	// Image Rects are always static
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to SimpleTextureProgram
	glEnableVertexAttribArray(0);
	// Feed texture coordinate Data to openGL
	glBindBuffer(GL_ARRAY_BUFFER, tBuffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), coords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

RenderableImageRect::~RenderableImageRect()
{
	// Delete buffer
	glDeleteBuffers(1, &tBuffer);
	// Texture is not managed by us, so do not delete
}

void RenderableImageRect::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	GraphManager::UseNewTexture0(texture);
	//GraphManager::UseNewTexture0(dynamic_cast<Canvas*>(parentInterface)->GetShadowMap());	// __Debug Shadow Map: Useless since we cannot directly display a depth map
	// Bind Data, parameters already using openGL coordinates
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	// Feed in texture rotation
	glUniform1f(3, textureRotation);
	// Transparency
	glUniform1f(4, transparency);

	// Render the image quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void RenderableImageRect::UpdateTexture(GLuint inTexture)
{
	texture = inTexture;
}

void RenderableImageRect::TextureRotationU(float angle)
{
	textureRotation = angle;
}