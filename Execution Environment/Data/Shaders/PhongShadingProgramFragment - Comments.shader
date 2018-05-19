/* In order to provide the ability to use either a color or a texture, we can either: provide different combinations of shaders, which means provide a bunch of meaningless shaders;
Or provide mask texture as color inputs(i.e. the shader expect both a color and a texture, while when only color is needed we can provide a white texture, when texture is needed we can provide a white color).*/

#version 430 core

// Output
layout (location = 0) out vec4 color;

// Input from vertex shader
in VertexOut
{
	vec3 PointLightVector;
	vec3 SunLightVector;
	vec3 SkyUpDirection;
	vec3 View;
	vec2 texCoord;
	vec4 shadowCoord;
} vertexData;

// Material Properties
layout (location = 5) uniform vec3 skyColor;
layout (location = 17) uniform vec3 groundColor;
layout (location = 6) uniform vec3 sunLightColor;
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
//layout(binding = 4) uniform sampler2DShadow shadowMap;
layout(binding = 4) uniform sampler2D shadowMap;

// Main Function: Do lighing in local tangent space
void main(void)
{
	// Normalize the incoming vectors, since they are interpreted
	vec3 V = normalize(vertexData.View);
	vec3 LP = normalize(vertexData.PointLightVector);	// D for Point
	vec3 LS = normalize(vertexData.SunLightVector);	// S for Sun
	vec3 LF = normalize(vertexData.SkyUpDirection);	// F for fill
	
	// Read the normal from the normal map and normalize it.
	vec3 Normal = normalize(texture(normalMap, vertexData.texCoord).rgb * 2.0 - vec3(1.0));
	//vec3 Normal = vec3(0,0,1);	// Debug: Unrotated normal direction
	
	// Calculate R locally
	vec3 RP = reflect(-LP, Normal);
	vec3 RS = reflect(-LS, Normal);
	
	// Compute Environment Light Contribution
	// Cosine between normal and direction to upper hemisphere 
	// 1 - normal is oriented to upper hemisphere
	// -1 - normal is oriented to lower hemisphere
	float cosNormalLight = dot(Normal, LF);
	// Shift from [-1,1] to [0,1] range
	float lightInfluence = cosNormalLight * 0.5 + 0.5;
	// interpolate colors from upper and lower hemispheres
	vec3 fillColor = mix(groundColor, skyColor, lightInfluence);
	
	// Compute the diffuse component for each fragment
	vec3 diffuseContribution = texture(diffuseMap, vertexData.texCoord).rgb * diffuseColor; 
	vec3 diffuse = (max(dot(Normal, LP), 0.0) * diffuseContribution * pointLightColor)
		+ (max(dot(Normal, LS), 0.0) * diffuseContribution * sunLightColor);	// Multiple or Add pointLightColor?
		// Pending making things simpler since currently to create a material is not very intuitive because too many color terms
		
	// Compute the specular component for each fragment
	vec3 specularContribution = specularColor;
	float specularModule = pow(max(dot(RP, V), 0.0), specularPower * texture(specularMap, vertexData.texCoord).r);	// Shininess or roughness: Notice we only use R channel of the map
	vec3 specular = specularModule * specularContribution * pointLightColor;
		//+specularModule * specularContribution * sunLightColor // Not Used
	
	// Compute the emissive component
	vec3 emissiveContribution = texture(emissiveMap, vertexData.texCoord).rgb * emissiveColor;
	vec3 emissive = emissiveContribution;
	
	// Write final color to the framebuffer
	color = vec4(max(dot(Normal, LS), 0.0) * diffuseContribution * sunLightColor, 1.0);
	//@color = vec4(fillColor + diffuse + specular + emissive, 1.0);
	//@color = vec4(diffuse, 1.0) * textureProj(shadowMap, vertexData.shadowCoord);
	//@float depth = textureProj(shadowMap, vertexData.shadowCoord);
	//color = vec4(depth, depth, depth, 1.0f);
	//@color = vec4(diffuse* textureProj(shadowMap, vertexData.shadowCoord), 1.0) ;
	//@
	float visibility = 1.0;
	if ( texture( shadowMap, vertexData.shadowCoord.xy ).x  <  vertexData.shadowCoord.z)
	{
		visibility = 0.2;
	}
	//color = vec4(visibility, visibility, visibility, 1.0f);
}

/* There are two ways to sample a DepthMap:
	One is to treat it as a normal texture and sample as usual
	The other is when Texture Comparison is enabled, we can use textureProj to sample 4x, which can make it smoother but highly texeled
	In either way, depth bias is applied using glPolygonOffset, since that is more elegant
*/

//@
	/*
	float visibility = 1.0;
	vec2 poissonDisk[4] = vec2[](
		vec2( -0.94201624, -0.39906216 ),
		vec2( 0.94558609, -0.76890725 ),
		vec2( -0.094184101, -0.92938870 ),
		vec2( 0.34495938, 0.29387760 ));
	for (int i=0;i<4;i++)
	{
		if ( texture( shadowMap, vertexData.shadowCoord.xy + poissonDisk[i]/3000.0 ).x  <  vertexData.shadowCoord.z )
		{
			visibility-=0.2;
		}
	}
	color = vec4(visibility, visibility, visibility, 1.0f);
	*/
