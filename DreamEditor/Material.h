#pragma once
#include "Shaders.h"
#include <stdlib.h>
#include <glm\matrix.hpp>
#include <glm\mat4x4.hpp>

// A Generic Class For Containing All Information About Three Possible Shaders Available
// Texture Coordinate isn't stored by us
// Usage: When a MeshSpec is awaken, its related material will be waken up as well; When a MeshSpec sleep, it will call its material to sleep as well
class Material
{
public:
	Material(Programs type, const float* inColor);	// Shape Material
	Material(Programs type, const char* inImagePath); // Texture Material
	Material(Programs type, const float* inGroundColor, const float* inPointLightColor,
		const float* inPointLightLocation, 	const float* inDiffuseColor, 
		const float* inSpecularColor, const float* inSpecularPower, 
		const float* inEmissiveColor, const char* inDiffuseTexturePath,	
		const char* inSpecularTexturePath, const char* inEmissiveTexturePath,
		const char* inNormalTexturePath);	// Phone Material; Expect NULL to indicate use default
	Material(Programs type, const char* inEnvBaseTexturePath, const char* inEnvTopTexturePath,
		const char* inGlossTexturePath);	// SEM Material: Expect NULL to indicate use default
	~Material();	// Delete Path Strings

	// Setup Render States
	void RenderSetting(glm::mat4& world, glm::mat4& view, glm::mat4& projection);
	Programs GetShaderType();

	// No need to do a reference count since we are unique: we are used by only one specific MeshSpec
	// For Simple Texture Program, inDiffuseTexturePath is used for the image
	void WakeUp();
	void FallAsleep();

private:
	// AMBIENT<EMISSION both want default NONE(Black), while DIFFUSE<SPECULAR might want default White.
	void LoadAttributeB(const char* attributeType, void* attributeProperty, const void* attributeValue, unsigned int size);	// Default NONE Color
	void LoadAttributeW(const char* attributeType, void* attributeProperty, const void* attributeValue, unsigned int size);	// Default Solid Color
	void Material::LoadStringAttribute(const char* attributeType, char** attributeProperty, const char* attributeValue);

private:
	// Type Classification
	Programs shaderType;
	GLuint shaderProgram;

	// Shader Data Fields
	float DiffuseColor[3];	// DiffuseColor for Phone Shader, Color for Shape Shader
	float GroundColor[3];
	float SpecularColor[3];
	float EmissiveColor[3];
	float PointLightColor[3];

	float SpecularPower = 0.f;
	float PointLightLocation[3];

	// Path Strings
	char* DiffuseTexturePath = NULL;
	char* SpecularTexturePath = NULL;	// SpecularMap for Phong Shader, GlossMap for MatShader
	char* EmissiveTexturePath = NULL;
	char* NormalTexturePath = NULL;
	char* EnvBaseTexturePath = NULL;
	char* EnvTopTexturePath = NULL;

private:
	// Shared OpenGL Resources
	GLuint DiffuseTexture = 0;	// DiffuseTex for Lamber Shader, ImageTex for Simple Texture Shader
	GLuint SpecularTexture = 0;
	GLuint EmissiveTexture = 0;
	GLuint NormalTexture = 0;
	GLuint EnvBaseTexture = 0;
	GLuint EnvTopTexture = 0;
};

/* For Different Materials
	- For Any Texture Files, There should be Only 1 unique existence in the openGL Memory
*/