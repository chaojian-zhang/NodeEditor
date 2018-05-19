//(RendeableMesh Property)
//v:	loc0 vec3 position
//		loc1 vec3 normal
//		loc2 vec3 tangent
//		loc3 vec2 texCoord
//		loc4 mat4x4 Model
//		loc14 mat4x4 View
//		loc15 mat4x4 Projection
//		loc16 mat4x4 Shadow
//
//		loc12 vec3 sunLightDirection - from canvas, dynamic	// In world space
//		loc13 vec3 pointLightLocation
#version 430 core

// Per-Vetex Inputs
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texCoord;

// Per Mesh Uniform
layout (location = 4) uniform mat4x4 Model;
layout (location = 14) uniform mat4x4 View;
layout (location = 15) uniform mat4x4 Projection;
layout (location = 16) uniform mat4x4 Shadow;
layout (location = 12) uniform vec3 sunLightDirection;
layout (location = 13) uniform vec3 pointLightLocation;

// Outputs
out VertexOut
{
	vec3 PointLightVector;
	vec3 SunLightVector;
	vec3 SkyUpDirection;
	vec3 View;	// View Vector
	vec3 ViewSpaceCoord;
	vec3 WorldSpaceCoord;
	vec2 texCoord;
	vec4 shadowCoord;
} vertexData;

// Main Program
void main(void)
{
	// Variables
	vec3 skyLightUpDirection = vec3(0.0, 1.0, 0.0);
	mat4x4 ModelView = View * Model;

	// Calculate view-space coordinate
	vec4 P = ModelView * vec4(position, 1);
	
	// Pass Two Coordinates
	vertexData.WorldSpaceCoord = vec3(Model * vec4(position ,1));
	vertexData.ViewSpaceCoord = vec3(P);
	
	// Calculate view-space normal, tangent; Notice that further lighting is now done in tagent space using fetched normals in normalMap as a transformer
	vec3 N = normalize(mat3(ModelView) * normal);	// We will do normalization in fragment shader again but we need normalized ones for later computation here
	vec3 T = normalize(mat3(ModelView) * tangent);
	// Calculate the bitangent vector from the normal and tangent vectors.
	vec3 B = cross(N, T);
	
	// Calculate point light vector
	vec3 pLight = vec3(ModelView * vec4(pointLightLocation, 1)) - P.xyz;
	vertexData.PointLightVector = vec3(dot(pLight, T), dot(pLight, B), dot(pLight, N));	// No need to normalize now
	
	// Calculate sun light vector
	vec3 sLight = mat3(View) * sunLightDirection;
	vertexData.SunLightVector = vec3(dot(sLight, T), dot(sLight, B), dot(sLight, N));	// No need to normalize now
	
	// Calculate sky light vector
	vertexData.SkyUpDirection = vec3(dot(skyLightUpDirection, T), dot(skyLightUpDirection, B), dot(skyLightUpDirection, N)); // No need to normalize now
	
	// Calculate view vector(simply the negative of the view-space position is not enough)
	vec3 V = -P.xyz;
	vertexData.View = vec3(dot(V, T), dot(V, B), dot(V, N));	// No need to normalize now
	
	// Pass the texture coordinate through unmodified
	vertexData.texCoord = texCoord;
	
	// Calculate Shadow Coord
	vertexData.shadowCoord = Shadow * vec4(position, 1);	// Notice use original position
	
	// Calculate the clip-space position of each vertex
	gl_Position = Projection * P;
}