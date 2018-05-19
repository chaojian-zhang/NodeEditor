#pragma once
#include "Renderable.h"

class Interface;
/* Provide a image square to serve as interface ioons
	- This kind of obejcts doesn't provide dynamic/transformable behaviros
*/
class RenderableImageIcon : public Renderable
{
public:
	// Manually feed texture coords so that we can combine different icons in a single map
	RenderableImageIcon(Interface* parent, float size, float coords[8], GLuint inTexture);	// openGL Second Quadrant
	~RenderableImageIcon();

	virtual void Render();

private:
	// Extended openGL objects
	GLuint tBuffer;
	GLuint texture;
};