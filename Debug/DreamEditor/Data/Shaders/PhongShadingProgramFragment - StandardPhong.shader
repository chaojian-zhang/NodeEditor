/* In order to provide the ability to use either a color or a texture, we can either: provide different combinations of shaders, which means provide a bunch of meaningless shaders;
Or provide mask texture as color inputs(i.e. the shader expect both a color and a texture, while when only color is needed we can provide a white texture, when texture is needed we can provide a white color).*/

#version 430 core

// Output
layout (location = 0) out vec4 color;

// Input from vertex shader
in VertexOut
{
	vec3 Normal;
	vec3 PointLightVector;
	vec3 DirectionalLightVector;
	vec3 View;
} vertexData;

// Material Properties
layout (location = 5) uniform vec3 skyColor;
layout (location = 16) uniform vec3 groundColor;
layout (location = 6) uniform vec3 directionalLightColor;
layout (location = 7) uniform vec3 pointLightColor;

layout (location = 8) uniform vec3 diffuseColor;
layout (location = 9) uniform vec3 specularColor;
layout (location = 10) uniform vec3 emissiveColor;
layout (location = 11) uniform float specularPower;

// Textures
layout(binding = 0) uniform sampler2D diffuseMap;
layout(binding = 1) uniform sampler2D specularMap;
layout(binding = 2) uniform sampler2D emissiveMap;
layout(binding = 3) uniform sampler2D normalMap;
// layout(binding = 4) uniform sampler2D environmentMap;

// Main Function
void main(void)
{
	// Normalize the incoming vectors
	vec3 N = normalize(vertexData.Normal);
	vec3 V = normalize(vertexData.View);
	vec3 LP = normalize(vertexData.PointLightVector);	// D for Point
	vec3 LS = normalize(vertexData.DirectionalLightVector);	// S for Sun
	
	// Calculate R locally
	vec3 RP = reflect(-LP, N);
	vec3 RS = reflect(-LS, N);
	
	// Compute Environment Light Contribution
	vec3 skyLightUpDirection = vec3(0.0, 1.0, 0.0);
	// cosine between normal and direction to upper hemisphere 
	// 1 - normal is oriented to upper hemisphere
	// -1 - normal is oriented to lower hemisphere
	float cosNormalLight = dot(N, skyLightUpDirection);
	// Shift from [-1,1] to [0,1] range
	float lightInfluence = cosNormalLight * 0.5 + 0.5;
	// interpolate colors from upper and lower hemispheres
	vec3 fillColor = mix(groundColor, skyColor, lightInfluence);
	
	// Compute the diffuse and specular components for each fragment
	vec3 diffuse = max(dot(N, LP), 0.0) * diffuseColor * pointLightColor;	// Multiple or Add?
		//+ max(dot(N, LS), 0.0) * diffuseColor * directionalLightColor;
	vec3 specular = pow(max(dot(RP, V), 0.0), specularPower) * specularColor * pointLightColor;
	
	// Write final color to the framebuffer
	color = vec4(fillColor + diffuse + specular + emissiveColor, 1.0);
}