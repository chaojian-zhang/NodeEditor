#version 430 core

// 3D position is necessary since we will use it in 3D space as well
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;
layout(location = 2) in int mapID;
layout(location = 3) uniform mat4x4 MVP;

out vec2 tc;
flat out int id;

void main()
{
	gl_Position = MVP * vec4(position, 1);
	tc = textureCoord;
	id = mapID;
}