#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 3) in vec2 texCoord;	// Same as Phong, because this is implemented in REnderable3DMesh object Vertex Array Obejct

layout (location = 3) uniform mat4x4 Model;
layout (location = 4) uniform mat4x4 View;
layout (location = 5) uniform mat4x4 Projection;

out VertexData
{
	vec3 normal;
	vec3 view;
	vec2 texCoord;
} vertexData;

void main(void)
{
	// Temp
	mat4 modelView = View * Model;
	vec4 posView = modelView * vec4(position, 1);
	
	// Pass Data
	vertexData.normal = mat3(modelView) * normal;
	vertexData.view = posView.xyz;
	vertexData.texCoord = texCoord;
	
	// Output Vertex
	gl_Position = Projection * posView;
}