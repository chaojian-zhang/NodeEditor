#pragma once
#include "Renderable.h"

/* Provide a round cornerd Frame(Border Only) with many configurations*/
class RenderableRoundCornerFrame : public Renderable
{
public:
	// When cornersize is zero, it indicated a rectangluar shape
	RenderableRoundCornerFrame(float width, float height, float* bcolor, float cornersize, bool d, bool render = true, float inOpacity = 1.0f);

	virtual void Render();
	void UpdateBuffer(float width, float height, float cornersize);
	void UpdateProperty(float* bcolor, bool render = true, float inOpacity = 1.0f);

private:
	bool bRender = true;	// Can be toggled not to render
	bool bDynamic;
	float opacity = 1.0f;
	float borderColor[3];	// Border color
};