#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) uniform mat4x4 MVP;
void main()
{
	gl_Position = MVP * vec4(position, 1);
}