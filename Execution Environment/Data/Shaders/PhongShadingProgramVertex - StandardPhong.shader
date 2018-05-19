//(RendeableMesh Property)
//v:	loc0 vec3 position
//		loc1 vec3 normal
//		loc2 vec3 tangent
//		loc3 vec2 texCoord
//		loc4 mat4x4 Model
//		loc14 mat4x4 View
//		loc15 mat4x4 Projection
//
//		loc12 vec3 directionalLightDir - from canvas, dynamic
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
layout (location = 12) uniform vec3 directionalLightDir;
layout (location = 13) uniform vec3 pointLightLocation;

// Outputs
out VertexOut
{
	vec3 Normal;
	vec3 PointLightVector;
	vec3 DirectionalLightVector;
	vec3 View;
} vertexData;

// Main Program
void main(void)
{
	mat4x4 ModelView = View * Model;

	// Calculate view-space coordinate
	vec4 P = ModelView * vec4(position, 1);
	
	// Calculate view-space normal
	vertexData.Normal = mat3(ModelView) * normal;
	
	// Calculate light vector
	vertexData.PointLightVector = vec3(ModelView * vec4(pointLightLocation, 1)) - P.xyz;
	vertexData.DirectionalLightVector = mat3(View) * directionalLightDir;
	
	// Calculate view vector(simply the negative of the view-space position)
	vertexData.View = -P.xyz;
	
	// Calculate the clip-space position of each vertex
	gl_Position = Projection * P;
}