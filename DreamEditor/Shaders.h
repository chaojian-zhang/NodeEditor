#pragma once
#include <gl\glew.h>

enum Programs
{
	TextProgram = 0,	// Provide Color, MVP, TexCoord, CharMapID, VertPosition and Transparency Input; And of course charmaps are bind as texture binding points
	ShapeProgram,	// With alpha channel animation available: Provide Vert, Color, MVP and Timescale as alpha channel
	SimpleTextureProgram,	// Provide vert, texCoord, MVP and a texture binding point
	PhongShadingProgram,	// Provide vert, texCoord, MVP, a texture binding point, and a point light property	// Actually this should be called Phone Lighting Model
	SEMShadingProgram,	// Spherical Environment Map
	NumberOfPrograms
};

class ShaderPath
{
public:
	static char* Programs[NumberOfPrograms * 2];
};

/* Program Layouts:
-Text
v:	loc0 vec3 position
	loc1 vec2 vertcoord
	loc2 int mapID
	loc3 mat4x4 MVp
f:	loc4 vec3 color
	loc5 float trans
	bin0 sam2D tex0
	bin1 sam2D tex1

-Shape
v:	loc0 vec3 position
	loc1 mat4x4 MVP
f:	loc2 vec3 color
	loc3 float timescale -> Transparancy, since mainly used for animation, we called it timescale

-SimpleTextured
v:	loc0 vec3 position
	loc1 vec2 texCoord
	loc2 mat4x4 MVP
f:	bin0 sam2D tex0	// Transparency embeded in texture already
	loc3 float rotation	// in U Axis
	loc4 float transparency

-Phong
(RendeableMesh Property)
v:	loc0 vec3 position
	loc1 vec3 normal
	loc2 vec3 tangent
	loc3 vec2 texCoord
	loc4 mat4x4 Model
	loc14 mat4x4 View
	loc15 mat4x4 Projection
	loc16 mat4x4 Shadow

	loc12 vec3 sunLightDirection - from canvas, dynamic	// In world space
	loc13 vec3 pointLightLocation

(Material Property)
f:	loc5 vec3 skyColor
	loc17 vec3 groundColor
	loc6 vec3 sunLightColor	- from canvas, dynamic
	loc7 vec3 pointLightColor

	loc8 vec3 diffuseColor
	loc9 vec3 specularColor
	loc10 vec3 emissiveColor
	loc11 float specularPower;

	bin0 sam2D diffuseMap
	bin1 sam2D specularMap
	bin2 sam2D emissiveMap
	bin3 sam2D normalMap
	bin4 sam2DShadow shadowMap

-SEM
v:	loc0 vec3 position
	loc1 vec3 normal
	loc3 vec2 texCoord

	loc3 mat4x4 Model
	loc4 mat4x4 View
	loc5 mat4x4 Projection

f:	bin0 sam2D environmentBase
	bin1 sam2D environmentTop
	bin2 sam2D glossMap

*/