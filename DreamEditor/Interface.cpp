#include "Interface.h"
#include "GraphManager.h"

#define ORTHOCAMERADISTANCE 1

bool Interface::CheckFallIn(double x, double y)
{
	if (x >= 0 && x <= viewportW && y <= 0 && y >= -(int)viewportH)
	{
		return true;
	}
	return false;
}

void Interface::UpdateProjection()
{
	view2D = glm::inverse(glm::translate(glm::mat4(1), glm::vec3(0, 0, ORTHOCAMERADISTANCE)));
	project2D = glm::ortho<float>(0.f, (float)viewportW, -(float)viewportH, 0.f, 0.f, 100.f);	// Notice that we need to use <float> otherwise it can cause problem when passing integers as parameters: the output will be nothing
}

glm::mat4x4& Interface::GetViewMatrix()
{
	return view2D;
}

glm::mat4x4& Interface::GetProjectMatrix()
{
	return project2D;
}

void* Interface::operator new (size_t size)
{
	// __Debug__: Might not be portable
	return _aligned_malloc(size, 16);
}

void Interface::operator delete (void *p)
{
	_aligned_free(p);
}