#version 430 core

// Provide 1 texture bindings
layout(binding = 0) uniform sampler2D texture0;
layout(location = 3) uniform float rotationU;
layout(location = 4) uniform float transparency;

in vec2 tc;
out vec4 fcolor;

void main()
{
	vec4 color = texture(texture0,vec2(tc.x+rotationU, tc.y));	// color.a is a valid componnet
	fcolor = vec4(color.rgb, color.a * transparency);
	//fcolor = vec4(tc.y, tc.y, tc.y , 1);	// Debug TC
	//float value = texture(texture0,tc); fcolor = vec4(value, value, value , 1);	// Debug ShadowMap
}