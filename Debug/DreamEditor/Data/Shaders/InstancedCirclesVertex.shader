#version 430 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 shapePosition;
layout(location = 2) in vec3 shapeColor;

layout(location = 3) uniform mat4x4 VP;
layout(location = 4) uniform vec3 highlightColor;
layout(location = 5) uniform unsigned int highlightID;

out vec3 color;

void main()
{
	if(highlightID == instanceID)
	{
		color = highlightColor;
	}
	else
	{
		color = shapeColor;
	}
	gl_Position = VP * vec4(vertPosition + shapePosition, 1);
}