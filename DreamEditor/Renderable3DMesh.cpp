#include "Renderable3DMesh.h"
#include "GraphManager.h"
#include "Canvas.h"
#include "Interface.h"

// Renderabls are positioned by calling UpdateLocation() explicitly, not in the constructor
Renderable3DMesh::Renderable3DMesh(void* vertexData, unsigned int vertexSize, void* indexData, unsigned int indexSize, void* texcoordData, unsigned int texcoordSize, Material* inMaterial)
	:material(inMaterial), buffers{}
{
	// Generate OpenGL Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, buffers);
	bufferUsed = 3;

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);

	// Generate Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_STATIC_DRAW);

	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);	// Always static
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to SimpleTextureProgram
	glEnableVertexAttribArray(0);

	if (texcoordData)
	{
		// Feed texture coordinate Data to openGL
		glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ARRAY_BUFFER, texcoordSize, texcoordData, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);	// __Pending: Now we are wasting one float for each vertex
		glEnableVertexAttribArray(1);
	}

	// Useful Information
	indexCount = indexSize / sizeof(unsigned int);	//Draw Indexed
}

Renderable3DMesh::Renderable3DMesh(void* vertexData, unsigned int vertexSize, void* normalData, void* tangetData,
	void* indexData, unsigned int indexSize, void* texcoordData, unsigned int texcoordSize, Material* inMaterial)
	:material(inMaterial), buffers{}
{
	// Generate OpenGL Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(5, buffers);
	bufferUsed = 5;

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);

	// Generate Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indexData, GL_STATIC_DRAW);

	// Generate Vertex Buffer: Location
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW);	// Always static
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to PhoneShaderProgram
	glEnableVertexAttribArray(0);

	// Generate Vertex Buffer: Normal
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, vertexSize, normalData, GL_STATIC_DRAW);	// Always static
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to PhoneShaderProgram
	glEnableVertexAttribArray(1);

	// Generate Vertex Buffer: Tangent
	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, vertexSize, tangetData, GL_STATIC_DRAW);	// Always static
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to PhoneShaderProgram
	glEnableVertexAttribArray(2);

	if (texcoordData)
	{
		// Feed texture coordinate Data to openGL
		glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
		glBufferData(GL_ARRAY_BUFFER, texcoordSize, texcoordData, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);	// __Pending: Now we are wasting one float for each vertex
		glEnableVertexAttribArray(3);
	}

	// Useful Information
	indexCount = indexSize / sizeof(unsigned int);	//Draw Indexed
}

Renderable3DMesh::~Renderable3DMesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(bufferUsed, buffers);
}

void Renderable3DMesh::Render(glm::mat4& world, glm::mat4& view, glm::mat4& projection)
{
	// Render Meshes with shader type Phong
	if (material->GetShaderType() == PhongShadingProgram)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);	// Enable polygon offset to resolve depth-fighting isuses
		static float configure1 = 2.0f, configure2 = 4.0f;// Debug
		glPolygonOffset(configure1, configure2);
	}
	/*
	"offset = m ¡¤ factor + r ¡¤ units
	where m is the maximum depth slope of the polygon (computed during
	rasterization), and r is the smallest value guaranteed to produce a
	resolvable difference in depth values and is an implementation-specific
	constant. Both factor and units may be negative."	-- Red Book

	"factor
	Specifies a scale factor that is used to create a variable depth offset for each polygon. The initial value is zero.
	units
	Specifies a value that is multiplied by an implementation-specific value to create a constant depth offset. The initial value is 0." -- MSDN

	"factor is the threashod for the polygon mesh angle, units is the actual offset scale, the offset direction is aways from the viewer"	-- Tom
	*/
	glEnable(GL_CULL_FACE); // Enable Cull face for Meshes
	glFrontFace(GL_CCW);// Set Face Winding order for meshes imported

	// Set up states
	glBindVertexArray(VAO);
	material->RenderSetting(world, view, projection);
	// If SimpleTexture Shader, then clear UV rotation
	if (material->GetShaderType() == SimpleTextureProgram)
	{
		glUniform1f(3, 0.f);
	}

	// Render
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	// Set State Back
	//glFrontFace(GL_CW);
	glDisable(GL_CULL_FACE);
	glDisable(GL_POLYGON_OFFSET_FILL);
}