#pragma once
#include <glm\matrix.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "InterfaceElement.h"
#include "Locales.h"	// I would like to use "Locale.h" but VS already got a header with that name

// VERY IMPORTANT: EACH VIEWPORT SPECIFY A NEW OPENGL SPACE FOR ITS INTERFACE ELEMENTS TO FOLLOW.
// Every coordinate input should be in Viewport relative OpenGL Second Quad.

class Interface
{
public:
	// Render Request
	virtual void Render() = 0;
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods){};
	virtual void OnResizeFramebuffer(int width, int height) = 0;	// Children Override to implement their own functionalities

	// Utility
	bool CheckFallIn(double x, double y);
	void UpdateProjection();

	// Can be overriden by Canvas to provide 3D datas
	virtual glm::mat4x4& GetViewMatrix();
	virtual glm::mat4x4& GetProjectMatrix();

	// alignment
	static void* operator new (size_t size);
	static void operator delete (void *p);

public:
	bool bHidden = false;

public:	// Public for Easy Access
	// Interface Only draw relative to its viewport area
	unsigned int viewportX, viewportY, viewportW, viewportH;	// Notice viewport is specified in first quadrant, and begin at Lower-Left corner of window

protected:
	glm::mat4x4 project2D;
	glm::mat4x4 view2D;	// Define Fundamental View Distance for 2D Interface
};