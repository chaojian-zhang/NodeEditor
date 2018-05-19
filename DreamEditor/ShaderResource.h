#pragma once
#include <gl\glew.h>
#include "Shaders.h"
#include "Constants.h"
#include <stdlib.h>

/* ShaderResource will be resopnsible for compiling and storing all the shaders that will be used in the program
	If a shader is already compiled and stored in binary form, ShaderResource will be responsible for loading it
*/
class ShaderResource
{
public:
	ShaderResource();	// Call CompileShader() to compile programs
	~ShaderResource();	// Delete Programs

	GLuint GetShader(Programs program);
	GLuint GetAppIconMap();

private:
	unsigned int GetShaderSource(unsigned int index, unsigned int tempSlot);	// After call this function we need to delete the tempBuffer;
	char* tempBuffers[2];

private:
	bool bCached = false;
	GLuint shaders[NumberOfPrograms];

	// Incidentally, we may as well take care of the Icon map
	GLuint iconMap;
};