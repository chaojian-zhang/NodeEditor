#pragma once
#include "Renderable.h"

/* Provides an ideal Quote Frame with an narrow head, border is always rendered, round corner using unified radius*/
class Renderable3DQuoteFrame : public Renderable
{
public:
	Renderable3DQuoteFrame(float width, float height, bool border, float* bcolor, float* acolor, float cornersize, bool d, float inOpacity = 1.0f);

	virtual void Render();
	void UpdateBuffer(float width, float height, float cornersize);
	void UpdateProperty(bool border, float* bcolor, float* acolor, float inOpacity = 1.0f);

private:
	float opacity = 1.0f;
	bool bBorder = true;
	bool bDynamic;
	float borderColor[3];	// Border color
	float areaColor[3];	// Interior color

	glm::mat4x4 modelResize;
};