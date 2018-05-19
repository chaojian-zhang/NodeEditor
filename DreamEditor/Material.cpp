#include "Material.h"
#include "MeshSpec.h"
#include "GraphManager.h"
#include "Constants.h"
#include "SystemWrapper.h"
#include "MaterialTextures.h"

Material::Material(Programs type, const float* inColor)
{
	// Assign Values
	shaderType = type;
	shaderProgram = GraphManager::shaderResource->GetShader(shaderType);

	// Load Color Attribute
	LoadAttributeW("Color", DiffuseColor, inColor, sizeof(float)* 3);
}

Material::Material(Programs type, const char* inImagePath)
{
	// Assign Values
	shaderType = type;
	shaderProgram = GraphManager::shaderResource->GetShader(shaderType);

	// Load Attribute
	LoadStringAttribute("ImageTexture", &DiffuseTexturePath, inImagePath);
}

Material::Material(Programs type, const float* inGroundColor, const float* inPointLightColor,
	const float* inPointLightLocation, const float* inDiffuseColor,
	const float* inSpecularColor, const float* inSpecularPower,
	const float* inEmissiveColor, const char* inDiffuseTexturePath,
	const char* inSpecularTexturePath, const char* inEmissiveTexturePath,
	const char* inNormalTexturePath)
{
	// Assign Values
	shaderType = type;
	shaderProgram = GraphManager::shaderResource->GetShader(shaderType);

	// Load Attribtues
	LoadAttributeW("DiffuseColor", DiffuseColor, inDiffuseColor, sizeof(float) * 3);
	LoadAttributeB("GroundColor", GroundColor, inGroundColor, sizeof(float) * 3);	// __Pending__ Defualt GroundFill might want to use ground texture color
	LoadAttributeW("SpecularColor", SpecularColor, inSpecularColor, sizeof(float) * 3);
	LoadAttributeB("EmissiveColor", EmissiveColor, inEmissiveColor, sizeof(float) * 3);
	LoadAttributeW("PointLightColor", PointLightColor, inPointLightColor, sizeof(float) * 3);

	LoadAttributeW("SpecularPower", &SpecularPower, inSpecularPower, sizeof(float));
	LoadAttributeB("PointLightLocation", PointLightLocation, inPointLightLocation, sizeof(float)*3);

	// Load Paths
	LoadStringAttribute("DiffuseTexture", &DiffuseTexturePath, inDiffuseTexturePath);
	LoadStringAttribute("SpecularTexture", &SpecularTexturePath, inSpecularTexturePath);
	LoadStringAttribute("EmissiveTexture", &EmissiveTexturePath, inEmissiveTexturePath);
	LoadStringAttribute("NormalTexture", &NormalTexturePath, inNormalTexturePath);

	// Redirect Paths that doesn't exist, Indicating use related Colors instead
	// For Diffuse, if we have texture, then color is just a full-pass white, otherwise the defaultDiffuseMask should be a full-pass white
	if (DiffuseTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultColorTextureMaskPath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		DiffuseTexturePath = (char*)calloc(1, stringSize);

		memcpy(DiffuseTexturePath, defaultPath, stringSize);
	}
	else
	{
		DiffuseColor[0] = 1.f;
		DiffuseColor[1] = 1.f;
		DiffuseColor[2] = 1.f;
	}

	// For Specular, if a texture is used, then the Specular Power severs as a maximum value because they are multiplied together; Otherwise the specular map should be a full whilte pass
	if (SpecularTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultSpecularTextureMaskPath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		SpecularTexturePath = (char*)calloc(1, stringSize);

		memcpy(SpecularTexturePath, defaultPath, stringSize);
	}

	// For Emission, if we have texture, then color is just a full-pass white, otherwise the defaultEmissionMask should be a full-pass white
	// __Pending__ Since this is the same as Diffuse Texture, we might not want to use a seperate texture object
	if (EmissiveTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultEmissiveTextureMaskPath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		EmissiveTexturePath = (char*)calloc(1, stringSize);

		memcpy(EmissiveTexturePath, defaultPath, stringSize);
	}
	else
	{
		EmissiveColor[0] = 1.f;
		EmissiveColor[1] = 1.f;
		EmissiveColor[2] = 1.f;
	}


	if (NormalTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultNormalTexturePath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		NormalTexturePath = (char*)calloc(1, stringSize);

		memcpy(NormalTexturePath, defaultPath, stringSize);
	}
}

Material::Material(Programs type, const char* inEnvironmentBasePath, const char* inEnvironmentTopPath,
	const char* inGlossTexturePath)
{
	// Assign Values
	shaderType = type;
	shaderProgram = GraphManager::shaderResource->GetShader(shaderType);

	// Load Paths
	LoadStringAttribute("EnvBaseTexture", &EnvBaseTexturePath, inEnvironmentBasePath);
	LoadStringAttribute("EnvTopTexture", &EnvTopTexturePath, inEnvironmentTopPath);
	LoadStringAttribute("GlossMap", &SpecularTexturePath, inGlossTexturePath);

	// Use Default Paths that doesn't exist
	// Default EnvBase will be a Matte
	if (EnvBaseTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultEnvTexturePath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		EnvBaseTexturePath = (char*)calloc(1, stringSize);

		memcpy(EnvBaseTexturePath, defaultPath, stringSize);
	}

	// Default EnvTop will be the same Matte, but we still assign it here rather than copy default EnvBase to make things consistent
	if (EnvTopTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultEnvTexturePath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		EnvTopTexturePath = (char*)calloc(1, stringSize);

		memcpy(EnvTopTexturePath, defaultPath, stringSize);
	}

	// For Specular, default a full whilte pass
	if (SpecularTexturePath == NULL)
	{
		// Use a Default Mask Texture
		const char* defaultPath = ApplicationPath::DefaultSpecularTextureMaskPath;
		unsigned int stringSize = strlen(defaultPath) + 1;
		SpecularTexturePath = (char*)calloc(1, stringSize);

		memcpy(SpecularTexturePath, defaultPath, stringSize);
	}
}

Material::~Material()
{
	// Delete Path Strings
	if (DiffuseTexture!=0)
		delete DiffuseTexturePath;
	if (SpecularTexturePath != 0)
		delete SpecularTexturePath;
	if (EmissiveTexturePath != 0)
		delete EmissiveTexturePath;
	if (NormalTexturePath != 0)
		delete NormalTexturePath;
	if (EnvBaseTexturePath != 0)
		delete EnvBaseTexturePath;
	if (EnvTopTexturePath != 0)
		delete EnvTopTexturePath;
}

void Material::RenderSetting(glm::mat4& world, glm::mat4& view, glm::mat4& projection)
{
	GraphManager::UseNewProgram(shaderProgram);

	// Parameter: __PEnding__ Only do calculation when necessary so put inside switch, but VS is complaining
	static const glm::mat4 scaleBias =
		glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	glm::mat4 shadow = scaleBias * Canvas::GetSunProjection() * Canvas::GetSunView() * world;

	switch (shaderType)
	{
	case ShapeProgram:
		glUniform3fv(2, 1, DiffuseColor);
		glUniform1f(3, 1.f);
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(projection* view *world));	// MVP
		break;
	case SimpleTextureProgram:
		GraphManager::UseNewTexture0(DiffuseTexture);
		glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection* view *world));	// MVP
		break;
	case PhongShadingProgram:
		// Light Properties_Vertex
		glUniform3fv(12, 1, Canvas::GetSunDirection());
		glUniform3fv(13, 1, PointLightLocation);

		// Matrices_Vertex
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(world));	// Model
		glUniformMatrix4fv(14, 1, GL_FALSE, glm::value_ptr(view));	// View
		glUniformMatrix4fv(15, 1, GL_FALSE, glm::value_ptr(projection));	// Projection

		// Light Properties_Fragment
		glUniform3fv(5, 1, Canvas::GetSkyColor());
		glUniform3fv(17, 1, GroundColor);
		glUniform3fv(6, 1, Canvas::GetSunColor());
		glUniform3fv(7, 1, PointLightColor);

		// Material Properties_Fragment
		glUniform3fv(8, 1, DiffuseColor);
		glUniform3fv(9, 1, SpecularColor);
		glUniform3fv(10, 1, EmissiveColor);
		glUniform1f(11, SpecularPower);

		// Texture Bindings_Fragment
		GraphManager::UseNewTexture0(DiffuseTexture);
		GraphManager::UseNewTexture1(SpecularTexture);
		GraphManager::UseNewTexture2(EmissiveTexture);
		GraphManager::UseNewTexture3(NormalTexture);

		// Feed Shadow Matrix and Bind Shadow Texture
		glUniformMatrix4fv(16, 1, GL_FALSE, glm::value_ptr(shadow));	// Shadow
		GraphManager::UseNewTexture4(Canvas::GetShadowMap());
		break;
	case SEMShadingProgram:
		// Matrices_Vertex
		glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(world));	// Model
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(view));	// View
		glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(projection));	// Projection

		// Texture Bindings_Fragment
		GraphManager::UseNewTexture0(EnvBaseTexture);
		GraphManager::UseNewTexture1(EnvTopTexture);
		GraphManager::UseNewTexture2(SpecularTexture);
		break;
	default:
		cout << "[Error]Error Detected When Set Up Materials: Wrong Shader Type" << endl;
		break;
	}
}

Programs Material::GetShaderType()
{
	return shaderType;
}

void Material::WakeUp()
{
	char absPath[MAX_PATH] = {};
	switch (shaderType)
	{
	case ShapeProgram:
		// No need to do anything
		break;
	case SimpleTextureProgram:
		// Load the specified Texture
		DiffuseTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(DiffuseTexturePath, absPath));
		break;
	case PhongShadingProgram:
		// Load Textures
		DiffuseTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(DiffuseTexturePath, absPath));
		SpecularTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(SpecularTexturePath, absPath));
		EmissiveTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(EmissiveTexturePath, absPath));
		NormalTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(NormalTexturePath, absPath));
		break;
	case SEMShadingProgram:
		// Load Textures
		EnvBaseTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(EnvBaseTexturePath, absPath));
		EnvTopTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(EnvTopTexturePath, absPath));
		SpecularTexture = MaterialTextures::GetTexture(MeshSpec::FindResourceFile(SpecularTexturePath, absPath));
		break;
	default:
		cout << "[Error]Error Detected When Set Up Materials: Wrong Shader Type" << endl;
		break;
	}
}

void Material::FallAsleep()
{
	char absPath[MAX_PATH] = {};
	switch (shaderType)
	{
	case ShapeProgram:
		// Do Nothing
		break;
	case SimpleTextureProgram:
		// Return the texture
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(DiffuseTexturePath, absPath));
		break;
	case PhongShadingProgram:
		// Return Textures
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(DiffuseTexturePath, absPath));
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(SpecularTexturePath, absPath));
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(EmissiveTexturePath, absPath));
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(NormalTexturePath, absPath));
		break;
	case SEMShadingProgram:
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(EnvBaseTexturePath, absPath));
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(EnvTopTexturePath, absPath));
		MaterialTextures::ReturnTexture(MeshSpec::FindResourceFile(SpecularTexturePath, absPath));
		break;
	default:
		cout << "[Error]Error Detected When Set Up Materials: Wrong Shader Type" << endl;
		break;
	}
}

void Material::LoadAttributeB(const char* attributeType, void* attributeProperty, const void* attributeValue, unsigned int size)
{
	// If not found 
	if (attributeValue == NULL)
	{
		// Assign a Default
		if (size == 12)
		{
			((float*)attributeProperty)[0] = 0.0f;
			((float*)attributeProperty)[1] = 0.0f;
			((float*)attributeProperty)[2] = 0.0f;
		}
		else
		{
			*((float*)attributeProperty) = 0;
		}

		// Debug Output
		MeshSpec::PrintNotFoundAttributeUseDefault(attributeType);
	}
	else
	{
		memcpy(attributeProperty, attributeValue, size);
	}
}

void Material::LoadAttributeW(const char* attributeType, void* attributeProperty, const void* attributeValue, unsigned int size)
{
	// If not found 
	if (attributeValue == NULL)
	{
		// Assign a Default
		if (size == 12)
		{
			((float*)attributeProperty)[0] = 1.0f;
			((float*)attributeProperty)[1] = 1.0f;
			((float*)attributeProperty)[2] = 1.0f;
		} 
		else
		{
			*((float*)attributeProperty) = 1;
		}

		// Debug Output
		MeshSpec::PrintNotFoundAttributeUseDefault(attributeType);
	}
	else
	{
		memcpy(attributeProperty, attributeValue, size);
	}
}

void Material::LoadStringAttribute(const char* attributeType, char** attributeProperty, const char* attributeValue)
{
	// If not found 
	if (attributeValue == NULL)
	{
		// Use NULL, we have special usage for this
		*attributeProperty = NULL;

		// Debug Output
		MeshSpec::PrintNotFoundAttributeUseDefault(attributeType);
	}
	else
	{
		// Create a space
		unsigned int stringSize = strlen((const char*)attributeValue) + 1;
		*attributeProperty = (char*)calloc(1, stringSize);

		memcpy(*attributeProperty, attributeValue, stringSize);
	}
}