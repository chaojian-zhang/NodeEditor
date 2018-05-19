#pragma once
#include "Renderable.h"
class Interface;
/* 2D Renderable string use case: interface elements, and interface text input box*/
/* Comment - Major Change Compared to DynamicText test:
	Summarize:
		Do no utilitze a intactCharsNumber to optimize string content update
	Details: 
		We are disregarding a firm coopration with TextField now e.g. intactNumChats etc to provide a generic facility to support text rendeirng.
		Those sort of optimization is actually possible to achieve through the interface element side, as long as it is clever enough to do the rendering and clamping moderately well,
		And only request information that neneded be updated.
		Anyway if we need, we can make it a sperate class. 
*/
class RenderableString : public Renderable
{
public:
	// Comment: To utilize 2D string in text box and text field, one does feed the full string into there, but upon display, use a viewport trick and translation to limit text display range
	RenderableString(Interface* parent, unsigned short* content, unsigned int numChars, float * inColor, unsigned int fontSize, bool dynamic, bool D3 = false);	// Specifically for labels and TextBoxes, in this case we will output final width and height; format of new line is not considered
	RenderableString(Interface* parent, unsigned short* content, unsigned int numChars, float * inColor, float fieldWidth, unsigned int fontSize, bool dynamic, bool D3 = false);	// For TextContainers, new line and line width are considered
	~RenderableString();	// Delete OpenGL objects

	virtual void Render();
	void UpdateBuffers_Line(unsigned short* content, unsigned int numChars, unsigned int fontSize = 32);	// Assuming dynamic
	void UpdateBuffers_Area(unsigned short* content, unsigned int numChars, float fieldWidth, unsigned int fontSize = 32);
	void UpdateProperty(float* inColor);
	void UpdateTransparancy(float value);

	float GetStringWidth();
	float GetStringHeight();

private:
	// Primitive Specific Parameters, used for rendering configuration
	float color[3];
	bool bDynamic;
	bool b3D;	// Enable pixel to meter sizing factor
	unsigned int renderNumCharacters;	// Bookkeeper for Render call glDrawArrays
	float transparancy = 1.f;

	// String is different in that we might require the renderable to provide dimension data
	float stringWidth = 0.f, stringHeight = 0.f;

private:
	// Extended openGL obejcts
	GLuint buffers[2];	// textureCoordinate and Map ID
};


/* Note: Assume 1 meter is equivalent to 1024 Pixels when 1:1 scale,
Since 1024 pixels can at most accomodate 1024/32=32 characters,
Then 1 meter space should accomodate 32 characters as well. That is, a 1/1024 scale.
*/
