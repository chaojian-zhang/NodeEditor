#version 430 core

layout(location = 4) uniform vec3 color;
layout(location = 5) uniform float trans;
// Provide Upto 2 texture bindings
layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D texture1;

flat in int id;
in vec2 tc;
out vec4 fcolor;

void main()
{
	switch(id)
	{
	case 0:
		fcolor = vec4(color.rgb,trans) * texture(texture0,tc).r;
		break;
	case 1:
		fcolor = vec4(color.rgb,trans) * texture(texture1,tc).r;
		break;
	default:
		fcolor = vec4(color.rgb,trans);
		break;
	}
}