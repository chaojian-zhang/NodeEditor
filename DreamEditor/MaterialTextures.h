#pragma once
#include <stdlib.h>
#include <GL\glew.h>

struct TextureSlot	// Used for Quadratic Probing
{
	// Data Info: Initialized to be 0
	GLuint textureObject = 0;	// Store GLuint texture objects
	unsigned int referenceCount = 0;		// 0 indicate not used yet, i.e. an empty slot

	// MetaInfo
	char* originalPath = NULL;	// Unique Copy of orignal file path

	// Hash Table Flags
};

// MaterialTextures provide a way to manage all textures used by materials, since we are using Texts to refer to specific materials
// The Class is used for easier and efficient memory access and loading and deloading textures used by different materials
// The Textures Here include those for meshes and application Icons
// ImageNodes have their own management system
class MaterialTextures
{
public:
	static void DeleteTextureResources(); // Destroty OpenGL Texture Objects in the array members if not used
	// Location Should be relative to cwd or abs // The user is reponsible for converting all the path to abs before calling this function(CWD or Program Dir or other external ones)
	static GLuint GetTexture(const char* texturePath);	// Always return the correct value for the texture
	// Input can be relative or abs path, but should be a functioning path, i.e. we are not responsible for dereferencing the path
	// ALL USERS OF THIS FUNCTION MUSTEN'T RELEASE THE OPENGL TEXTURE OBJECT
	static void ReturnTexture(const char* texturePath);

private:
	// Index is hashed SID
	static TextureSlot textureSlots[1000];	// For a 4Gb GPU, we can only store 256 textures, thus 1000 would be a 1:4 density for our hash table
};

// Logic: The Object is static and called upon DeleteTextureResources() when the application ends //