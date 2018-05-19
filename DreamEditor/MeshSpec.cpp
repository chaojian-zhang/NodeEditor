#define _CRT_SECURE_NO_WARNINGS
#include "MeshSpec.h"
#include <thread>
#include <iostream>
#include "GraphManager.h"
#include "TextSpecParser.h"
using std::thread;
using std::cout;
using std::cin;
using std::endl;

MeshSpec::MeshSpec(FILE* file)
{
	// Extract Filename Length
	unsigned int filePathSize;
	fread(&filePathSize, sizeof(unsigned int), 1, file);
	// Extract Filename
	char* fileLocation = (char*)calloc(1, filePathSize + 1);	// Notice we appended a NULL here
	fread(fileLocation, filePathSize, 1, file);

	LoadMeshSpec(fileLocation);

	// Delete String
	free(fileLocation);
}

MeshSpec::MeshSpec(const char* meshSpecFileLocation)
{
	LoadMeshSpec(meshSpecFileLocation);
}

MeshSpec::~MeshSpec()
{
	free(specFileLocation);
	free(shapeFileLocation);
	free(collisionFileLocation);
	free(meshName);

	if (mesh)
	{
		delete mesh;
	}
	if (material)
	{
		delete material;
	}
	if (soup)
	{
		delete soup;
	}
}

void MeshSpec::RequireReference()
{
	referenceCount++;
}

void MeshSpec::ReleaseReference()
{
	if (referenceCount == 0)
	{
		__debugbreak();
		cout << "[Error]Unbalanced MeshSpec data reference detected!" << endl;
	}
	referenceCount--;

	// // Now if No One is using us and we still have renderables, we are gonna release them
	// This shouldb't happen if releaseMeshUsage is called as required, but we cannot guarantee
	if (referenceCount == 0 && mesh!= NULL)
	{
		// Destory the Mesh
		delete mesh;
		mesh = NULL;
		// Destory the collision data
		delete soup;
		soup = NULL;

		// Let The Material Sleep
		material->FallAsleep();
	}
}

// Request: OpenGL Obejct Creation
Renderable3DMesh* MeshSpec::RequireMeshUsage()
{
	// If we have mesh, then return mesh
	if (mesh != NULL)
	{
		meshUsageCount++;
		bAlreadyLoading = false;	// Clear Load State
		return mesh;
	}
	// Otherwise load mesh, and return null
	else
	{
		// Dispatch a Thread to do the loading if we are not already doing so
		if (!bAlreadyLoading)	// Stage One: Load from Harddisk
		{
			// Dispatch a working thread to load the mesh
			thread(Stream, this).detach();	// detach it because we do not care when it ends

			// Display a user information
			GraphManager::canvas->UpdateInformationLabelText((unsigned short*)Canvas_Information_LoadingMesh_Begin);

			// __Debug__ Since multi-threading isn't working now due to context cannot be shared. I am just calling it directly to verify the code itself is working
			//Stream(this);
			bAlreadyLoading = true;
		}
		if (bFinalizeLoadStage)	// Stage Two: Hard diskloading is done, we can load it into GPU now
		{
			// Load mesh into GPU
			if (material->GetShaderType() == PhongShadingProgram || material->GetShaderType() == SEMShadingProgram)
			{
				mesh = new Renderable3DMesh(tempMeshObject->mVertices, tempVertexDataSize, tempMeshObject->mNormals, tempMeshObject->mTangents,
					tempIndices, tempIndexDataSize, tempMeshObject->mTextureCoords[0], tempTexcoordSize, material);
			}
			else
			{
				mesh = new Renderable3DMesh(tempMeshObject->mVertices, tempVertexDataSize, tempIndices, tempIndexDataSize,
					tempMeshObject->mTextureCoords[0], tempTexcoordSize, material);
			}

			// Wake up the material in order to load textures
			material->WakeUp();

			// Display a user information
			GraphManager::canvas->UpdateInformationLabelText((unsigned short*)Canvas_Information_LoadingMesh_Finish);

			// Reset Flag
			bFinalizeLoadStage = false;
		}

		// Return NULL
		return NULL;
	}
}

void MeshSpec::ReleaseMeshUsage()
{
	if (meshUsageCount == 0)
	{
		__debugbreak();
		cout << "[Error]Unbalanced MeshSpec mesh usage detected!" << endl;
	}
	meshUsageCount--;

	// Destroy the objects when they are used by no one
	if (meshUsageCount == 0)
	{
		// Destory the Mesh
		delete mesh;
		mesh = NULL;
		// Destory the collision data
		delete soup;
		soup = NULL;

		// Let The Material Sleep
		material->FallAsleep();
	}
}

// Data Saving Related
unsigned int MeshSpec::GetReferenceCount()
{
	return referenceCount;
}

const char* MeshSpec::GetSpecFileLocation()
{
	return specFileLocation;
}

void MeshSpec::SetSavingSlot(unsigned short index)
{
	binarySavingSlot = index;
}

unsigned short MeshSpec::GetSavedSlot()
{
	return binarySavingSlot;
}

// Debug Usage
void MeshSpec::PrintNotFoundAttributeUseDefault(const char* Message)
{
	cout << "[Debug]Cannot Find Attribute::" << Message << ". Used Default." << endl;
}

// Mesh Streaming Logic
// Caution: Shoud call only thread-safe functions
// Note: A function is already thread safe if it doesn't modify non-local memory and it doesn't call any function that does.
void MeshSpec::Stream(MeshSpec* meshSpec)
{
	// Create The mesh
	{
		// Create an importer
		if (meshSpec->tempImporter)
		{
			delete meshSpec->tempImporter;
		}
		meshSpec->tempImporter = new Assimp::Importer();	// DOn't forget this!
		char absPath[MAX_PATH] = {};
		const aiScene* scene = meshSpec->tempImporter->ReadFile(FindResourceFile(meshSpec->shapeFileLocation, absPath), aiProcess_CalcTangentSpace | aiProcess_SortByPType | aiProcess_FlipUVs);	// The file is expected to be traingulated on Blender for faster loading
		if (scene == NULL || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) // && scene->mFlags == 0)
		{
			// __Debug__
			cout << "[Error] Cannot load mesh: " << meshSpec->tempImporter->GetErrorString() << endl;
			__debugbreak();
		}

		// Calculate Tangents, since this didn't work when ReadFile*(
		// importer.ApplyPostProcessing(aiProcess_CalcTangentSpace);
		//if (scene == NULL)
		//{
		//	// __Debug__
		//	cout << "[Error] Cannot load mesh: " << importer.GetErrorString() << endl;
		//	__debugbreak();
		//}

		// If we have at least one mesh and it is triangulated, then (prepare to) load it into openGL; Actually we are just loading the first mesh
		if (scene->mNumMeshes != 0 && scene->mMeshes[0]->mPrimitiveTypes | aiPrimitiveType_TRIANGLE)
		{
			// Typing Saver
			meshSpec->tempMeshObject = scene->mMeshes[0];

			// Construct indices data from our scene
			// Debug_temp - debug note: AssImp Seems not working pretty well eliminating verts and essentially useless for dae indices
			// Index Count
			unsigned int indexCount = meshSpec->tempMeshObject->mNumFaces * 3;
			meshSpec->tempIndexDataSize = indexCount * sizeof(unsigned int);
			// Copy Index Data into our temp index buffer
			if (meshSpec->tempIndices != NULL)
			{
				free(meshSpec->tempIndices);
			}
			meshSpec->tempIndices = (unsigned int*)malloc(meshSpec->tempIndexDataSize);
			for (unsigned int i = 0; i < meshSpec->tempMeshObject->mNumFaces; i++)
			{
				memcpy(meshSpec->tempIndices + i * 3, meshSpec->tempMeshObject->mFaces[i].mIndices, 3 * sizeof(unsigned int));	// 3 is also the value of mesh->mFaces[i].mNumIndices
			}

			// Create a Renderable Mesh for Advanced Shaders
			if (meshSpec->material->GetShaderType() == PhongShadingProgram || meshSpec->material->GetShaderType() == SEMShadingProgram)
			{
				// Extract Normal And Tangent Information
				if (!meshSpec->tempMeshObject->mNormals || !meshSpec->tempMeshObject->mTangents)
				{
					// __Debug__
					cout << "[Error] Cannot load mesh using Phone Shader without providing tangents and normals." << endl;
					__debugbreak();
				}

				meshSpec->tempVertexDataSize = meshSpec->tempMeshObject->mNumVertices * sizeof(aiVector3D);
				meshSpec->tempTexcoordSize = meshSpec->tempMeshObject->mNumVertices * sizeof(aiVector3D);
				/*meshSpec->mesh = new Renderable3DMesh(meshObject->mVertices, vertexDataSize, meshObject->mNormals, meshObject->mTangents,
					indices, indexDataSize, meshObject->mTextureCoords[0], texcoordSize, meshSpec->material);*/	// Called in second stage
			}
			// Otherwise create a Renderable Mesh for simple type shaders
			else
			{
				meshSpec->tempVertexDataSize = meshSpec->tempMeshObject->mNumVertices * sizeof(aiVector3D);
				meshSpec->tempTexcoordSize = meshSpec->tempMeshObject->mNumVertices * sizeof(aiVector3D);
				/*meshSpec->mesh = new Renderable3DMesh(meshObject->mVertices, vertexDataSize, indices, indexDataSize, 
					meshObject->mTextureCoords[0], texcoordSize, meshSpec->material);*/	// Called in second stage
			}			

			// Create Default Collision Object if not specified
			if (!meshSpec->collisionFileLocation)
			{
				meshSpec->soup = new Polysoup(indexCount, meshSpec->tempIndices, meshSpec->tempMeshObject->mNumVertices, (float*)(meshSpec->tempMeshObject->mVertices), true);
			}
		}
	}

	// __Pending__ Optimization:
	// If Collision is the same as Shape file, we can avoid loading it again
	// And we can do stremaing in a second thread

	// Create Collision Object if specified
	{
		if (meshSpec->collisionFileLocation)
		{
			// Create an importer
			Assimp::Importer importer;
			char absPath[MAX_PATH] = {};
			const aiScene* scene = importer.ReadFile(FindResourceFile(meshSpec->collisionFileLocation, absPath), aiProcess_SortByPType);
			if (scene == NULL || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
			{
				// __Debug__
				cout << "[Error] Cannot load mesh: " << importer.GetErrorString() << endl;
			}

			// If we have at least one mesh and it is triangulated, then load it into openGL
			if (scene->mNumMeshes != 0 && scene->mMeshes[0]->mPrimitiveTypes | aiPrimitiveType_TRIANGLE)
			{
				// Typing Saver
				aiMesh* meshObject = scene->mMeshes[0];

				// Index Count
				unsigned int indexCount = meshObject->mNumFaces * 3;
				unsigned int indexDataSize = indexCount * sizeof(unsigned int);
				// Copy Index Data into our temp index buffer
				unsigned int* indices = (unsigned int*)malloc(indexDataSize);
				for (unsigned int i = 0; i < meshObject->mNumFaces; i++)
				{
					memcpy(indices + i * 3, meshObject->mFaces[i].mIndices, 3 * sizeof(unsigned int));	// 3 is also the value of mesh->mFaces[i].mNumIndices
				}

				// Create a collision object
				unsigned int vertexDataSize = meshObject->mNumVertices * sizeof(aiVector3D);
				meshSpec->soup = new Polysoup(indexCount, indices, meshObject->mNumVertices, (float*)(meshObject->mVertices), false);
			}
		}
	}


	// Wake Up The Material As Well
	// meshSpec->material->WakeUp();	// Now in second stage, since our texture loading doesn't support multithread yet

	// Enter next stage
	meshSpec->bFinalizeLoadStage = true;
}

char* MeshSpec::FindResourceFile(const char* fileLocation, char* absFilePath)
{
	// First Search in Document Data Directory
	SystemFunctions::GetDocumentDataPath(fileLocation, absFilePath);
	if (SystemFunctions::IsFileExist(absFilePath))	// Thread safe
	{
		return absFilePath;
	}

	//// Second Search in Executable Data Directory
	SystemFunctions::GetExecutableDataPath(fileLocation, absFilePath);
	if (SystemFunctions::IsFileExist(absFilePath))	// Thread safe
	{
		return absFilePath;
	}

	// An Error Occured
	cout << "[Error]An error occured when trying to retrieve fileLocation, the file specified isn't in DataII directory." << endl;
	return NULL;
}

void MeshSpec::LoadMeshSpec(const char* meshSpecFileLocation)
{
	// Parse the Spec File
	char absPath[MAX_PATH] = {};
	TextSpecParser parser(FindResourceFile(meshSpecFileLocation, absPath));	// This is necessary since this function is used by MeshSpec(File* file), which can only get a path relative to Data

	// Store the meshSpecFile
	specFileLocation = (char*)calloc(1, strlen(meshSpecFileLocation) + 1);
	strcpy(specFileLocation, meshSpecFileLocation);

	// Temp Variables
	const int* tempInt = NULL;
	const char* string = NULL;
	const float* tempFloat = NULL;

	// Retrieve Useful General Attributes: If Not Assigned, Use Default
	// Shader Defaults are managed by Matrial Object, not our concern
	{
		// Get SpecFileName
		string = SystemFunctions::GetFilename(meshSpecFileLocation);	// Notice this "Filename" is different from "Filename", this is even purer because it is just a filename

		// Prepare for mesh name extraction
		int numChars = SystemFunctions::ANSIToU16(string);
		// Strip out the file format suffic
		int backCount = 0;
		for (int i = numChars-2; i > 0; i--)	// 1 for NULL char, 1 for index
		{
			backCount++;
			if (string[i] == '.')
				break;
		}
		//numChars -= backCount;	// numChars should be the actual number of character + 1, that 1 for NULL char in U16; But for some reason we cannot
		// assign a numChar that doesn't match  the actual string, otherwise the below ANSIToU16 will generate garbage
		// Store it as a Unicode
		meshName = (unsigned short*)malloc(numChars * 2);	// No need to calloc
		SystemFunctions::ANSIToU16(string, meshName, numChars);
		// Do a manual truncation here
		meshName[numChars - backCount - 1] = 0;

		// Get shape mesh name
		string = parser.GetString("ShapeMesh");
		if (!string)
		{
			// Obviously this loading is a failure but we cannot just fail it
			// __Pending
			__debugbreak();

			// One solution is to assign it a defualt name, then later when wake up this will point to a specialized mesh
			shapeFileLocation = NULL;	 // Obviously no name is more special than NULL

			// Debug
			PrintNotFoundAttributeUseDefault("ShapeMesh");
		}
		else
		{
			shapeFileLocation = (char*)calloc(1, strlen(string) + 1);
			strcpy(shapeFileLocation, string);
		}

		// Get collisionFileName
		string = parser.GetString("CollisionMesh");
		if (!string)
		{
			// Obviously this is quite expected
			collisionFileLocation = NULL;	 // Obviously no name is more special than NULL

			// Debug
			PrintNotFoundAttributeUseDefault("CollisionMesh");
		}
		else
		{
			collisionFileLocation = (char*)calloc(1, strlen(string) + 1);
			strcpy(collisionFileLocation, string);
		}

		// Get Collision Radius
		tempFloat = parser.GetFloat("Radius");
		radius = tempFloat ? (*tempFloat) : 1;

		// Get Shader Type
		tempInt = parser.GetInt("Shader");
		shader = tempInt ? static_cast<Programs>(*tempInt) : static_cast<Programs>(1);

		// Debug
		parser.PrintAttributeStructure();

		// Create A Material
		switch (shader)
		{
		case ShapeProgram:
			material = new Material(shader, parser.GetFloatArray("Color", 3));
			break;
		case SimpleTextureProgram:
			material = new Material(shader, parser.GetString("ImageTexture"));
			break;
		case PhongShadingProgram:
			material = new Material(shader, parser.GetFloatArray("GroundColor", 3), parser.GetFloatArray("PointLightColor", 3), parser.GetFloatArray("PointLightLocation", 3),
				parser.GetFloatArray("DiffuseColor", 3), parser.GetFloatArray("SpecularColor", 3), parser.GetFloat("SpecularPower"), parser.GetFloatArray("EmissiveColor", 3), 
				parser.GetString("DiffuseTexture"), parser.GetString("SpecularTexture"), parser.GetString("EmissiveTexture"), parser.GetString("NormalTexture"));	
			// I doubt how long this operation will take....; Fortunately our specification file for a mesh isn't long
			// We could try to hash it if it's too slow
			break;
		case SEMShadingProgram:
			material = new Material(shader, parser.GetString("EnvBaseTexture"), parser.GetString("EnvTopTexture"), parser.GetString("GlossTexture"));
			break;
		default:
			material = NULL;
			break;
		}
	}
}