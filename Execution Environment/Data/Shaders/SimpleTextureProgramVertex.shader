#version 430 core

// 3D position is necessary since we will use it in 3D space as well
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) uniform mat4x4 MVP;

out vec2 tc;

void main()
{
	gl_Position = MVP * vec4(position, 1);
	tc = texCoord;
}