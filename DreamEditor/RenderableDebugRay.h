#pragma once
#include "Renderable.h"

/* Provides an ideal Quote Frame with an narrow head, border is always rendered, round corner using unified radius*/
class RenderableDebugRay : public Renderable
{
public:
	RenderableDebugRay(glm::vec3 origin, glm::vec3 direction, float length);	// In World Space, i.e. Not in Object Space

	virtual void Render();
	void UpdateBuffer(glm::vec3 origin, glm::vec3 direction, float length);	// In World Space, i.e. Not in Object Space; Generate a ray
	void UpdateBuffer_Line(glm::vec3 origin, glm::vec3 endPoint);	// In World Space; Generate a Line

	static float color[3];
};