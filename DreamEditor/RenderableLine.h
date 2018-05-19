#pragma once
#include "Renderable.h"
class Interface;
/* Provide a line shape*/
class RenderableLine : public Renderable
{
public:
	RenderableLine(Interface* parent, float width, float height, float color[3], bool d);

	virtual void Render();
	void UpdateBuffer(float width, float height);	// Assume dynamic
	void UpdateProperty(float* color);

	float timeScale = 1;	// Used for easy change of Transparancy

private:
	bool bDynamic;	// Useful for validating UpdateBuffer() operation legitity
	float lineColor[3];
};