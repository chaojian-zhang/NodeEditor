#version 430 core

layout(location = 2) uniform vec3 color;
layout(location = 3) uniform float timeScale;

out vec4 fcolor;

void main()
{
	fcolor = vec4(color.rgb, timeScale);
}