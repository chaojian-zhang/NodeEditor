#pragma once
#include "Renderable.h"
class Interface;

/* Provide a image rect
	- This kind of obejcts doesn't provide dynamic/transformable behaviros
*/
class RenderableImageRect : public Renderable
{
public:
	// Manually feed texture coords so that we can combine different icons in a single map <-- No idea what its talking about
	RenderableImageRect(Interface* parent, float width, float height, const float coords[8], GLuint inTexture);
	//RenderableImageRect(Interface* parent, float coords[8], GLuint inTexture);	// For Canvas Interface Full Area Quad// Maight want to seperately implement this
	~RenderableImageRect();

	virtual void Render();
	void UpdateTexture(GLuint inTexture);
	void TextureRotationU(float angle);
	void SetTransparency(float value) { transparency = value; };

private:
	// Extended openGL objects
	GLuint tBuffer;
	GLuint texture;

	// Bookkeeping
	float textureRotation = 0.f;
	float transparency = 1.f;
};