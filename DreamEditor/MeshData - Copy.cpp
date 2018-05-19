#define _CRT_SECURE_NO_WARNINGS
#include "MeshData.h"
#include <thread>
#include <iostream>
#include "GraphManager.h"
using std::thread;
using std::cout;
using std::cin;
using std::endl;
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

MeshData::MeshData(char* meshSpecFilename)
{
	Load(meshSpecFilename);
}

MeshData::MeshData(FILE* file)
{
	// Extract Filename Length
	unsigned int filenameSize;
	fread(&filenameSize, sizeof(unsigned int), 1, file);

	// Extract Filename
	meshSpecFilename = (char*)malloc(filenameSize + 1);
	fread(meshSpecFilename, filenameSize, 1, file);
	Load(meshSpecFilename);
}

MeshData::~MeshData()
{
	free(meshSpecFilename);
	free(meshFileName);
	free(textureFileName);
	if (iconMapName)
	{
		free(iconMapName);
	}
	if (collisionName)
	{
		free(collisionName);
	}
	if (iconMapName)	// We musten't check iconTexture again 0 to determine whether we are using a customed texture, because iconTexture can be an application texture, which is also not zero
	{
		glDeleteTextures(1, &iconTexture);
	}
	if (mesh)
	{
		delete mesh;
	}

	free(meshFileNameU16);
}

void MeshData::Save(FILE* file)
{
	if (usageCount > 0)
	{
		// Tag a Block
		unsigned char tag = MESH_DATA_BLOCK_TAG;
		fwrite(&tag, sizeof(unsigned char),1 ,file);

		// Save Filename Length
		unsigned int nameLength = strlen(meshSpecFilename);
		fwrite(&nameLength, sizeof(unsigned int), 1, file);

		// Save Filename
		fwrite(meshSpecFilename, nameLength, 1, file);
	}
}

// Request: OpenGL Obejct Creation
Renderable3DMesh* MeshData::GetMesh()
{
	// If we have mesh, then return mesh
	if (mesh != NULL)
	{
		return mesh;
	}
	// Otherwise load mesh, and return null
	else
	{
		// Dispatch a Thread to do the loading if we are not already doing so
		if (!bAlreadyLoading)
		{
			thread(Stream, this).detach();	// detach it because we do not care when it ends
			bAlreadyLoading = true;
		}

		// Return NULL
		return NULL;
	}
}

void MeshData::UseMesh()
{
	usageCount++;
}

void MeshData::UnuseMesh()
{
	usageCount--;	// By desing this should not cause the usageCount to drop below 0(which would be 2^32 - 1), otherwise it would indicate a bug
}

// MeshSpec Loading Logic
void MeshData::Load(char* filename)
{
	// Open the spec file
	FILE* file = fopen(filename, "wb");
	// Check File Exist
	if (!file)
	{
		// _Debug_: Maybe we should do more about other things erro check, e.g. disable the creation of this object at all
		perror("[Error]Error Load Specified Mesh Spec File. File doesn't exist or other file system error.");
		return;
	}
	// Extract Useful Information
	char temp[4];
	// fmt identifier
	fread(temp, 3, 1, file);
	temp[3] = NULL;
	if (strcmp(temp, "MSF") != 0)
	{
		cout << "[Error]Error Load Specified Mesh Spec File: File binary format isn't recognized." << endl;
		return;
	}
	// fmt version
	fread(temp, 1, 1, file);
	if ((unsigned char)temp[0] != 1)
	{
		cout << "[Error]Error Load Specified Mesh Spec File: File version out of date." << endl;
		return;
	}
	// meshFilename
	fread(temp, 4, 1, file);
	unsigned int meshNameSize = *(unsigned int*)temp;
	meshFileName = (char*)malloc(meshNameSize + 1);
	meshFileName[meshNameSize] = NULL;
	fread(meshFileName, meshNameSize, 1, file);
	// iconCoords
	fread(iconCoord, sizeof(float)*8, 1, file);
	// shader type
	fread(&program, sizeof(Programs), 1, file);
	// pointLightPos
	fread(&pointLightPos, sizeof(Vector3f), 1, file);
	// textureFileName
	fread(temp, 4, 1, file);
	unsigned int textNameSize = *(unsigned int*)temp;
	textureFileName = (char*)malloc(textNameSize + 1);
	textureFileName[textNameSize] = NULL;
	fread(textureFileName, textNameSize, 1, file);
	// Collision file name
	fread(temp, 4, 1, file);
	unsigned int collisionNameSize = *(unsigned int*)temp;
	if (collisionNameSize != NULL)
	{
		collisionName = (char*)malloc(collisionNameSize + 1);
		collisionName[collisionNameSize] = NULL;
		fread(collisionName, collisionNameSize, 1, file);
	}
	// iconMapFileName
	fread(temp, 4, 1, file);
	unsigned int iconNameSize = *(unsigned int*)temp;
	if (iconNameSize != NULL)
	{
		iconMapName = (char*)malloc(iconNameSize + 1);
		iconMapName[iconNameSize] = NULL;
		fread(iconMapName, iconNameSize, 1, file);
	}
	// Get Icon Texture
	if (iconMapName == NULL)
	{
		iconTexture = GraphManager::shaderResource->GetAppIconMap();
	}
	else
	{
		// Debug_Temp: Pending a solution about filename path resolution: CWD or program path?
		iconTexture = GraphManager::LoadImageTexture(iconMapName);
	}

	fclose(file);

	// Provide a handy management of U16 filename for MeshCreationPopupMenu presentation
	unsigned int wcharNumChars = SystemFunctions::ANSIToU16(meshFileName);
	meshFileNameU16 = (unsigned short*)malloc(wcharNumChars);
	SystemFunctions::ANSIToU16(meshFileName, meshFileNameU16, wcharNumChars);
}

// Mesh Streaming Logic
void MeshData::Stream(MeshData* meshData)
{
	// Create an importer
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshData->meshFileName, aiProcess_SortByPType);	// The file is expected to be traingulated on Blender for faster loading
	if (scene == NULL || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) // && scene->mFlags == 0)
	{
		// __Debug__
		cout << "[Error] Cannot load mesh: " << importer.GetErrorString() << endl;
	}

	// If we have at least one mesh and it is triangulated, then load it into openGL; Actually we are just loading the first mesh
	if (scene->mNumMeshes != 0 && scene->mMeshes[0]->mPrimitiveTypes | aiPrimitiveType_TRIANGLE)
	{
		// Typing Saver
		aiMesh* meshObject = scene->mMeshes[0];

		// Construct indices data from our scene
		// Debug_temp - debug note: AssImp Seems not working pretty well eliminating verts and essentially useless for dae indices
		// Index Count
		unsigned int indexCount = meshObject->mNumFaces * 3;
		unsigned int indexDataSize = indexCount * sizeof(unsigned int);
		// Copy Index Data into our temp index buffer
		unsigned int* indices = (unsigned int*)malloc(indexDataSize);
		for (unsigned int i = 0; i < meshObject->mNumFaces; i++)
		{
			memcpy(indices + i * 3, meshObject->mFaces[i].mIndices, 3 * sizeof(unsigned int));	// 3 is also the value of mesh->mFaces[i].mNumIndices
		}

		// Load and Create Texture
		GLuint textrue = GraphManager::LoadImageTexture(meshData->textureFileName);

		// Create a Renderable Mesh
		unsigned int vertexDataSize = meshObject->mNumVertices * sizeof(aiVector3D);
		unsigned int texcoordSize = meshObject->mNumVertices * sizeof(aiVector3D);
		meshData->mesh = new Renderable3DMesh(meshObject->mVertices, vertexDataSize, indices, indexDataSize, meshObject->mTextureCoords[0], texcoordSize,
			meshData->program, textrue, &(meshData->pointLightPos));
	}
}

// Some Information on Join and Detach a thread:
// http://stackoverflow.com/questions/22426132/do-i-need-to-join-every-thread-in-my-application
// http://www.cplusplus.com/reference/thread/thread/join/
// http://www.cplusplus.com/reference/thread/thread/detach/?kw=detach
// http://www.cplusplus.com/reference/thread/thread/