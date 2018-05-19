#pragma once
#include "Renderable.h"

class Interface;
/* Provide a round left and right sides quad with not many configurations
	- Doesn't need a round(corner) size, since it will be a semi-circle
*/
class RenderableRoundSidesQuad : public Renderable
{
public:
	RenderableRoundSidesQuad(Interface* parent, float width, float height, bool border, float* bcolor, float* acolor, bool dynamic, bool d3);

	virtual void Render();
	// Support for dynamic and cinematic Interaction
	void UpdateBuffer(float width, float height);
	void UpdateProperty(bool border, float* bcolor, float* acolor);

private:
	// Primitive Specific Parameters, used for rendering configuration
	bool bBorder = true;
	bool bDynamic;
	float borderColor[3];	// Border color
	float areaColor[3];	// Interior color

	glm::mat4x4 modelResize;
};