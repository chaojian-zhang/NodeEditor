#pragma once
#include "Renderable.h"
class Interface;

/* Provide a irregular text highlight shape*/
class RenderableCursorHighlight : public Renderable
{
public:
	RenderableCursorHighlight(Interface* parent, float color[3]);	// Always dynamic; And considering the actual usage of this object, no need for initial parameters

	virtual void Render();
	// For single line, spanWidth is the actual width; For multiLine, spanWidth is the boundWidth
	// Forward and backward only used for multiline, as offset for the first and last rectangle
	// And that is an awkward design: since they are so specialzed, why not just use different functions?
	//void UpdateBuffer(float forward, float backward, float spanWidth, unsigned int numLines, float lineHeight);
	void UpdateBuffer_SingleLine(float spanWidth, float lineHeight);
	void UpdateBuffer_MultiLine(float forward, float backward, float spanWidth, unsigned int numLines, float lineHeight);
	void UpdateProperty(float trans, float* color = NULL);	// NULL indicate keep current

private:
	bool bSingleLine = true;
	float transparancy = 1;
	float highlightColor[3];
};