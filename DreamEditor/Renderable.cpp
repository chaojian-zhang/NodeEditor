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