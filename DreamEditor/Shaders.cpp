#include "Shaders.h"

char* ShaderPath::Programs[NumberOfPrograms * 2] =
{
	// Vertex Shaders
	"Data\\Shaders\\TextProgramVertex.shader",
	"Data\\Shaders\\ShapeProgramVertex.shader",
	"Data\\Shaders\\SimpleTextureProgramVertex.shader",
	"Data\\Shaders\\PhongShadingProgramVertex.shader",
	"Data\\Shaders\\SEMRoughSpecularVertex.shader",

	// Fragment Shaders
	"Data\\Shaders\\TextProgramFragment.shader",
	"Data\\Shaders\\ShapeProgramFragment.shader",
	"Data\\Shaders\\SimpleTextureProgramFragment.shader",
	"Data\\Shaders\\PhongShadingProgramFragment.shader",
	"Data\\Shaders\\SEMRoughSpecularFragment.shader",
};