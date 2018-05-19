#pragma once
#include <GL\glew.h>
#include <glm\matrix.hpp>
#include <glm\mat4x4.hpp>
#include <glm\vec4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#define ShadowMapWidth 2048
#define ShadowMapHeight 2048
class Canvas;

class Sun
{
public:
	Sun(Canvas* c);
	~Sun();

	GLuint GetMap();
	void UseFBO();

	float* GetColor(){ return sunColor; }
	float* GetSky(){ return skyFillColor; }
	float* GetDirection() { return sunLocation; }
	GLuint GetShadowMap(){ return shadowTexture; }
	//GLuint GetShadowFrameBuffer(){ return shadowFBO; }

	// Debug Use: Check Sun View
	glm::mat4& GetView() { return sunView; }
	glm::mat4& GetProjection() { return sunProjection; }

	// Sun Function
	void UpdateSun(float sunTiming); // Timing in degrees ranging [0, 360], indicating SE->NW movement

	// alignment
	static void* operator new (size_t size);
	static void operator delete (void *p);

private:
	GLuint shadowFBO;
	GLuint shadowTexture;
	Canvas* canvas;	// Quick Access

	// Sun Property
	float sunColor[3];
	float sunLocation[3];	// In world space
	//float sunDirection[3];		// The direction of the sun in world space is the inverse of the location // But due to the way our shader work, we no need to inverse that
	float skyFillColor[3];

	glm::mat4 sunProjection;
	glm::mat4 sunView;
};