#version 430 core

// Output
layout (location = 0) out vec4 color;

// Input from vertex shader
in VertexOut
{
	vec3 PointLightVector;
	vec3 SunLightVector;
	vec3 SkyUpDirection;
	vec3 View;	// 	View Vector
	vec3 ViewSpaceCoord;
	vec3 WorldSpaceCoord;
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
layout(binding = 1) uniform sampler2D specularMap;	// For Height map, 0(black) indicates zero surface, while 1(white) indicates maximum depth. This is contrarary to visual aspects but it algebraically make sense
layout(binding = 2) uniform sampler2D emissiveMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D shadowMap;

// Fog Support
uniform bool bFog = true;
//uniform vec3 fogColor = vec3(0.7, 0.8, 0.9);
//uniform float fogDistance = 25.0;	// 25m

// Parallax Mapping Parameter
uniform float parallaxScale = 0.085;	// Control Depth, the bigger the deeper; Too deep can cause artifacts; It is better to expose this value for controllable depth per object
//uniform  float parallaxScale = 0.1;
//uniform  float parallaxScale = 0.5;

vec3 ApplyFog(vec3 color)
{	// Using Formula: Fe = e^-zde, Fi = e^-zdi, where z is the distance to camera
// de is extinction, di is inscattering
	float z = length(vertexData.ViewSpaceCoord);
	float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - vertexData.WorldSpaceCoord.y);
	float di = 0.045 * smoothstep(0.0, 40.0, 20.0 - vertexData.WorldSpaceCoord.y);
	float extinction = exp(-z * de);
	float inscattering = exp(-z * di);
	vec3 fogColor = skyColor * 2;//  + vec3(0.2, 0.2, 0.25);
	return color * extinction + fogColor * (1.0 - inscattering);
}

// View in Tangent space, TCoord is original coord, parallaxHeight is .....
// The function returns new texture coordinates
vec2 ParallexMapping(in vec3 view, in vec2 TCoord, out float parallaxHeight)
{	
	//////////////////////////////////////////////////
	// Steep Parallax
	// determine number of layers from angle between view and normal
	const float minLayers = 5;
	const float maxLayers = 15;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), view)));

	// height of each layer
	float layerHeight = 1.0 / numLayers;
	// depth of current layer
	float currentLayerHeight = 0;
	// shift of texture coordinates for each iteration
	view.y = -view.y;	// Do this since our x(u in texture space) goes from left to right, while our y (V in  texture space) goes from up to down into negative axis
	vec2 dtex = parallaxScale * view.xy / view.z / numLayers;

	// current texture coordinates
	vec2 currentTextureCoords = TCoord;

	// get first depth from heightmap
	float heightFromTexture = texture(specularMap, currentTextureCoords).b;	// r for Specularity, g for transparancy, b for height

	// while point is above surface
	while(heightFromTexture > currentLayerHeight) 
	{
		// to the next layer
		currentLayerHeight += layerHeight;
		// shift texture coordinates along vector V
		currentTextureCoords -= dtex;
		// get new depth from heightmap
		heightFromTexture = texture(specularMap, currentTextureCoords).b;
	}
	
	// return results if do not continue to relief parallax
	//parallaxHeight = currentLayerHeight;
	//return currentTextureCoords;

	////////////////////////////////////////////////// continue from Steep Paralla
	// Relief Parallax
	// decrease shift and height of layer by half
	vec2 deltaTexCoord = dtex / 2;
	float deltaHeight = layerHeight / 2;

	// return to the mid point of previous layer
	currentTextureCoords += deltaTexCoord;
	currentLayerHeight -= deltaHeight;

	// binary search to increase precision of Steep Paralax Mapping
	const int numSearches = 5;
	for(int i=0; i<numSearches; i++)
	{
		// decrease shift and height of layer by half
		deltaTexCoord /= 2;
		deltaHeight /= 2;

		// new depth from heightmap
		heightFromTexture = texture(specularMap, currentTextureCoords).b;

		// shift along or agains vector V
		if(heightFromTexture > currentLayerHeight) // below the surface
		{
			currentTextureCoords -= deltaTexCoord;
			currentLayerHeight += deltaHeight;
		}
		else // above the surface
		{
			currentTextureCoords += deltaTexCoord;
			currentLayerHeight -= deltaHeight;
		}
   }

   // return results
   parallaxHeight = currentLayerHeight;    
   return currentTextureCoords;
	
	
	/////////////////////////////////////////////
	// Simple Parallax
	/*
	// get depth for this fragment
	float initialHeight = texture(specularMap, TCoord).b;

	// calculate amount of offset for Parallax Mapping
	vec2 texCoordOffset = parallaxScale * view.xy / view.z * initialHeight;

	// retunr modified texture coordinates
	return TCoord - texCoordOffset;*/

}

float parallaxSoftShadowMultiplier(in vec3 light, in vec2 initialTexCoord, in float initialHeight)
{
	float shadowMultiplier = 1;

	const float minLayers = 10;
	const float maxLayers = 15;

	// calculate lighting only for surface oriented to the light source
	if(dot(vec3(0, 0, 1), light) > 0)
	{
		// Invert y
		light.y = -light.y;
	
		// calculate initial parameters
		float numSamplesUnderSurface = 0;
		shadowMultiplier = 0;
		float numLayers	= mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), light)));
		float layerHeight = initialHeight / numLayers;
		vec2 texStep = parallaxScale * light.xy / light.z / numLayers;

		// current parameters
		float currentLayerHeight = initialHeight - layerHeight;
		vec2 currentTextureCoords = initialTexCoord + texStep;
		float heightFromTexture	= texture(specularMap, currentTextureCoords).b;
		int stepIndex = 1;

		// while point is below depth 0.0 )
		while(currentLayerHeight > 0)
		{
			// if point is under the surface
			if(heightFromTexture < currentLayerHeight)
			{
				// calculate partial shadowing factor
				numSamplesUnderSurface	+= 1;
				float newShadowMultiplier	= (currentLayerHeight - heightFromTexture) *
												 (1.0 - stepIndex / numLayers);
				shadowMultiplier	= max(shadowMultiplier, newShadowMultiplier);
			}

			// offset to the next layer
			stepIndex	+= 1;
			currentLayerHeight -= layerHeight;
			currentTextureCoords += texStep;
			heightFromTexture = texture(specularMap, currentTextureCoords).b;
		}

		// Shadowing factor should be 1 if there were no points under the surface
		if(numSamplesUnderSurface < 1)
		{
			shadowMultiplier = 1;
		}
		else
		{
			shadowMultiplier = 1.0 - shadowMultiplier;
		}
	}
	return shadowMultiplier;
}

// Credit: http://sunandblackcat.com/tipFullView.php?topicid=28

// Main Function: Do lighing in local tangent space
void main(void)
{
	// Normalize the incoming vectors, since they are interpreted
	vec3 V = normalize(vertexData.View);
	vec3 LP = normalize(vertexData.PointLightVector);	// D for Point
	vec3 LS = normalize(vertexData.SunLightVector);	// S for Sun
	vec3 LF = normalize(vertexData.SkyUpDirection);	// F for fill
	
	// Do parallex mapping
	float parallaxHeight;
	vec2 parallexTexCoord = ParallexMapping(V, vertexData.texCoord, parallaxHeight);	// Notice our function require a inverse direction of View
	
	// Read the normal from the normal map and normalize it.
	vec3 Normal = normalize(texture(normalMap, parallexTexCoord).rgb * 2.0 - vec3(1.0));
	//vec3 Normal = vec3(0,0,1);	// Debug: Unrotated normal direction: If normal is wrong, there will be strange patches on the object surface
	
	// Calculate R locally
	vec3 RP = reflect(-LP, Normal);
	vec3 RS = reflect(-LS, Normal);
	
	// Compute the diffuse component for each fragment	
	vec2 poissonDisk[64];
    poissonDisk[0] = vec2(-0.613392, 0.617481);
    poissonDisk[1] = vec2(0.170019, -0.040254);
    poissonDisk[2] = vec2(-0.299417, 0.791925);
    poissonDisk[3] = vec2(0.645680, 0.493210);
    poissonDisk[4] = vec2(-0.651784, 0.717887);
    poissonDisk[5] = vec2(0.421003, 0.027070);
    poissonDisk[6] = vec2(-0.817194, -0.271096);
    poissonDisk[7] = vec2(-0.705374, -0.668203);
    poissonDisk[8] = vec2(0.977050, -0.108615);
    poissonDisk[9] = vec2(0.063326, 0.142369);
    poissonDisk[10] = vec2(0.203528, 0.214331);
    poissonDisk[11] = vec2(-0.667531, 0.326090);
    poissonDisk[12] = vec2(-0.098422, -0.295755);
    poissonDisk[13] = vec2(-0.885922, 0.215369);
    poissonDisk[14] = vec2(0.566637, 0.605213);
    poissonDisk[15] = vec2(0.039766, -0.396100);
    poissonDisk[16] = vec2(0.751946, 0.453352);
    poissonDisk[17] = vec2(0.078707, -0.715323);
    poissonDisk[18] = vec2(-0.075838, -0.529344);
    poissonDisk[19] = vec2(0.724479, -0.580798);
    poissonDisk[20] = vec2(0.222999, -0.215125);
    poissonDisk[21] = vec2(-0.467574, -0.405438);
    poissonDisk[22] = vec2(-0.248268, -0.814753);
    poissonDisk[23] = vec2(0.354411, -0.887570);
    poissonDisk[24] = vec2(0.175817, 0.382366);
    poissonDisk[25] = vec2(0.487472, -0.063082);
    poissonDisk[26] = vec2(-0.084078, 0.898312);
    poissonDisk[27] = vec2(0.488876, -0.783441);
    poissonDisk[28] = vec2(0.470016, 0.217933);
    poissonDisk[29] = vec2(-0.696890, -0.549791);
    poissonDisk[30] = vec2(-0.149693, 0.605762);
    poissonDisk[31] = vec2(0.034211, 0.979980);
    poissonDisk[32] = vec2(0.503098, -0.308878);
    poissonDisk[33] = vec2(-0.016205, -0.872921);
    poissonDisk[34] = vec2(0.385784, -0.393902);
    poissonDisk[35] = vec2(-0.146886, -0.859249);
    poissonDisk[36] = vec2(0.643361, 0.164098);
    poissonDisk[37] = vec2(0.634388, -0.049471);
    poissonDisk[38] = vec2(-0.688894, 0.007843);
    poissonDisk[39] = vec2(0.464034, -0.188818);
    poissonDisk[40] = vec2(-0.440840, 0.137486);
    poissonDisk[41] = vec2(0.364483, 0.511704);
    poissonDisk[42] = vec2(0.034028, 0.325968);
    poissonDisk[43] = vec2(0.099094, -0.308023);
    poissonDisk[44] = vec2(0.693960, -0.366253);
    poissonDisk[45] = vec2(0.678884, -0.204688);
    poissonDisk[46] = vec2(0.001801, 0.780328);
    poissonDisk[47] = vec2(0.145177, -0.898984);
    poissonDisk[48] = vec2(0.062655, -0.611866);
    poissonDisk[49] = vec2(0.315226, -0.604297);
    poissonDisk[50] = vec2(-0.780145, 0.486251);
    poissonDisk[51] = vec2(-0.371868, 0.882138);
    poissonDisk[52] = vec2(0.200476, 0.494430);
    poissonDisk[53] = vec2(-0.494552, -0.711051);
    poissonDisk[54] = vec2(0.612476, 0.705252);
    poissonDisk[55] = vec2(-0.578845, -0.768792);
    poissonDisk[56] = vec2(-0.772454, -0.090976);
    poissonDisk[57] = vec2(0.504440, 0.372295);
    poissonDisk[58] = vec2(0.155736, 0.065157);
    poissonDisk[59] = vec2(0.391522, 0.849605);
    poissonDisk[60] = vec2(-0.620106, -0.328104);
    poissonDisk[61] = vec2(0.789239, -0.419965);
    poissonDisk[62] = vec2(-0.545396, 0.538133);
    poissonDisk[63] = vec2(-0.178564, -0.596057);
	
	float visibility = 1.0;
	for (int i=0;i<64;i++)	// This is too much for mid-spec machines we might need to reduce it
	{
		if ( texture( shadowMap, vertexData.shadowCoord.xy + poissonDisk[i]/1500.0).x  <  vertexData.shadowCoord.z )
		{
			visibility -= 1.0 / 64.0;	//visibility-=1/64; Not working
		}
	}
	vec3 diffuseAlbeto = texture(diffuseMap, parallexTexCoord).rgb;
	vec3 diffuseContribution = diffuseAlbeto * diffuseColor * visibility; 
	//float dis = length()
	//float pointAttenuation = 1.0 / (1.0 + 0.1*dist + 0.01*dist*dist);
	vec3 diffuse = (max(dot(Normal, LP), 0.0) * diffuseContribution * pointLightColor)
		+ (max(dot(Normal, LS), 0.0) * diffuseContribution * sunLightColor);	// 0.25 is strength control
		
	// Compute Environment Fill Light Contribution
	float cosNormalLight = dot(Normal, LF);
	float lightInfluence = cosNormalLight * 0.5 + 0.5;
	vec3 fillColor = mix(groundColor, skyColor, lightInfluence) * diffuseAlbeto; // 0.25 for strength control 
		
	// Compute the specular component for each fragment
	// Shininess/roughness: Notice we only use R channel of the map
	vec3 specularContribution = specularColor;
	vec4 specularData = texture(specularMap, parallexTexCoord);	// r for Specularity, g for transparancy, b for height
	float specularModulePoint = pow(max(dot(RP, V), 0.0), specularPower * specularData.r);	
	float specularModuleSun = pow(max(dot(RS, V), 0.0), specularPower * specularData.r);	
	vec3 specular = specularModulePoint * specularContribution * pointLightColor
		+specularModuleSun * specularContribution * sunLightColor;
	
	// Compute the emissive component
	vec3 emissiveContribution = texture(emissiveMap, parallexTexCoord).rgb * emissiveColor;
	vec3 emissive = emissiveContribution;
	
	// Get self-shadowing factor from parallax mapping
	// float shadowMultiplier = parallaxSoftShadowMultiplier(LS, vertexData.texCoord, parallaxHeight - 0.05);
	float shadowMultiplier = 1;
	
	// Write final color to the framebuffer
	if(bFog)
	{
		color = vec4(ApplyFog(fillColor + shadowMultiplier * (diffuse + specular) + emissive), specularData.g);
	}
	else
	{
		color = vec4(fillColor + shadowMultiplier * (diffuse + specular) + emissive, specularData.g);
	}
	// Debug
	//float repeatValue = texture(specularMap, vertexData.texCoord).r;
	//float repeatValue = specularPower * specularData.r;
	//float repeatValue = texture(specularMap, vertexData.texCoord).r;
	//color = vec4(repeatValue,repeatValue, repeatValue, 1.0);
	//color = vec4(specular, 1.0);
}