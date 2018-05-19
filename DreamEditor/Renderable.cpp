#include "Renderable.h"
#include "GraphManager.h"

Renderable::Renderable(Programs inProgram, Interface* parent)
	:parentInterface(parent)
{
	// Create OpenGL Objects for the Renderable
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vBuffer);

	program = GraphManager::shaderResource->GetShader(inProgram);
}

Renderable::~Renderable()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &vBuffer);

	// Program is not managed by us, borrowed from ShaderResource
}

// Interfaces
//void Renderable::Transformation(float x, float y, float z = 0, float rx = 0, float ry = 0, float rz = 0, float sx = 0, float sy = 0, float sz = 0)
//{
//	// Update Properties
//	translation = glm::vec3(x, y, z);
//	rotation = glm::rotate(glm::mat4(1), rx, glm::vec3(1, 0, 0));
//	rotation = glm::rotate(rotation, ry, glm::vec3(0, 1, 0));
//	rotation = glm::rotate(rotation, rz, glm::vec3(0, 0, 1));
//	scaling = glm::vec3(sx, sy, sz);
//
//	// Update Transformation
//	UpdateMatrix();
//}

void* Renderable::operator new (size_t size)
{
	// __Debug__: Might not be portable
	return _aligned_malloc(size, 16);
}

void Renderable::operator delete (void *p)
{
	_aligned_free(p);
}
// Reference: http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16