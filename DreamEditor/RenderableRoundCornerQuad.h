#pragma once
#include "Renderable.h"
class Interface;
/* Provide a round cornerd quad with many configurations*/
// Caution this type is not directly compatible with 3D use, since border genration is using pixel size
class RenderableRoundCornerQuad : public Renderable
{
public:
	// Argument named y2 insead of yoff to emphasis it should be an absolute value, i.e. negative in most cases, rather than an offset
	// When cornersize is zero, it indicated a rectangluar shape
	RenderableRoundCornerQuad(Interface* parent, float width, float height, bool border, float* bcolor, float* acolor, float cornersize, bool d, bool d3, float inOpacity = 1.0f);

	virtual void Render();
	void UpdateBuffer(float width, float height, float cornersize);
	void UpdateProperty(bool border, float* bcolor, float* acolor, float inOpacity = 1.0f);

private:
	// Primitive Specific Parameters, used for rendering configuration
	float opacity = 1.0f;
	bool bBorder = true;
	bool bDynamic;
	float borderColor[3];	// Border color
	float areaColor[3];	// Interior color

	glm::mat4x4 modelResize;
};