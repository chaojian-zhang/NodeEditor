#version 430 core
layout (binding = 0) uniform sampler2D environmentBase;	// Environment of Mat
layout (binding = 1) uniform sampler2D environmentTop;	// Blurred EnvironmentMap
layout (binding = 2) uniform sampler2D glossMap;	// Roughness

in VertexData
{
	vec3 normal;
	vec3 view;
	vec2 texCoord;
} vertexData;

layout (location = 0) out vec4 color;

void main(void)
{
	// Normalize view vector
	vec3 v = normalize(vertexData.view);
	
	// Reflect v about the plane defined by the normal at the fragment
	vec3 r = reflect(v, normalize(vertexData.normal));
	
	// Compute scale factor
	float m = 2. * sqrt( pow( r.x, 2. ) + pow( r.y, 2. ) + pow( r.z + 1., 2. ) );
	vec2 envCoord = r.xy / m + .5;
	
	// Sample gloss factor from glossmap texture
	float gloss = texture(glossMap, vertexData.texCoord).r;
	
	// Sample from environment maps
	vec3 base = texture2D( environmentBase, envCoord ).rgb;
	vec3 topping = texture2D( environmentTop, envCoord ).rgb;
	vec3 combine = mix(topping, base, gloss);	// Gloss defined the amout of base
	
	// Output Color
	color = vec4(combine, 1.0);
	//color = vec4(texture(glossMap, vertexData.texCoord).rgb, 1.0);	// Debug GlossMap
	//color = vec4(vertexData.texCoord.y, vertexData.texCoord.y, vertexData.texCoord.y, 1.0);	// Test Texture Coordinate
}