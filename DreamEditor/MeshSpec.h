#pragma once
#include "Shaders.h"
#include "Renderable3DMesh.h"
#include <stdio.h>
#include "MathElegent.h"
#include "SystemWrapper.h"	// FOr string Conversoin
#include "Material.h"
#include "Polysoup.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

// For MeshSpec, We Expect Just a Filename, and when MeshSpec object is created, we expect it to in either ExeData or DocData
// For Resource names in MeshSpec(Shape, Collision, Texture), it should be relative to Data
class  MeshSpec
{
public:
	MeshSpec(FILE* file);
	MeshSpec(const char* meshSpecFileLocation);	// Load The Information Specified In The Spec File into the object; Expect pure form
	~MeshSpec();

	// Connection Functions
	void RequireReference();
	void ReleaseReference();	// For Any User(Specifically, MeshNode), it must call those two function in pair.

	// Usage Functions
	Renderable3DMesh* RequireMeshUsage();	// The User Shall Call This Function Constantly Untill He Get A Non-NULL
	void ReleaseMeshUsage();
	// Called Everytime when usage
	Polysoup* GetPolySoup() { return soup; };

	// Data Saving Related
	unsigned int GetReferenceCount();
	const char* GetSpecFileLocation();
	void SetSavingSlot(unsigned short index);
	unsigned short GetSavedSlot();

	// Debug
	// A Very Small Utility
	static void PrintNotFoundAttributeUseDefault(const char* Message);

	// Property Inquiry Functions
	const Programs GetProgram() { return shader; }
	float GetRadius() { return radius; }
	unsigned short* GetName() { return meshName; }

public:
	// Used by ourselves and Material class
	static char* FindResourceFile(const char* fileLocation, char* absFilePath);	// Search and return file abs path; fileLocation is the relative location to Data directory

private:
	// Mesh Streaming Logic
	static void Stream(MeshSpec* spec);	// It gotta to be static so that we can create a thread of it
	// Caution: Even though this function is static, we should not make any member variable of MeshData static
	// because they are NOT
	// Caution: Do not call any Rendering thread functions on it, because openGL context is NOT shared

	// Here the input should be of the form "dirInData\\filename", that is, a "pure form"
	void LoadMeshSpec(const char* meshSpecFileLocation);

private:
	// General Information
	char* specFileLocation = NULL;	// ANSI terminate with null	// In pure form "dire\filename"
	unsigned short* meshName = NULL;	// Unicode Name For the Mesh, it is specFileName, but a U16 version
	// Resource fileNames
	char* shapeFileLocation = NULL;
	char* collisionFileLocation = NULL;

	// Mesh Information
	Renderable3DMesh* mesh = NULL;

	// Material Information
	Programs shader;	// Shader Program of choice
	Material* material = NULL;

	// Collision Information
	Polysoup* soup = NULL;	// Only available when the mesh is already loaded
	float radius;	// Specified in MeshSpec file so always available

private:
	// BookKeeping Usages
	unsigned int referenceCount = 0;	// In order to keep track how many MeshNodes are using this MeshSpec
	unsigned short binarySavingSlot = 0;	// Assigned by MeshResource when it's being saved, so that When later MeshNodes are saved they will know which slot the meshSpec is
	unsigned int meshUsageCount = 0;	// For Mesh Usage Management
	bool bAlreadyLoading = false;		// Streaming Timing: Streaming initiation
	bool bFinalizeLoadStage = false;	// Streaming into GPU

private:
	// Multithread supporting temporary variables, used in between the two loading stages
	Assimp::Importer* tempImporter = NULL;
	aiMesh* tempMeshObject = NULL;
	unsigned int* tempIndices = NULL;
	unsigned int tempIndexDataSize = 0;
	unsigned int tempVertexDataSize = 0;
	unsigned int tempTexcoordSize = 0;

};

/* Logic:
	- Each mesh node will keep a reference to the meshspec it uses, and (when rendering) acquire actual material setup and shape vertex input 
	from the meshSpec when wakeup and being rendered. The Important information a meshNode keeps is its transformation.
	- Each meshSpec will bookkeep its usages/sharing by different meshNodes, and load and upload mesh/texture/collision information only 
	when conditions are satisfied. The important a meshSpec keeps is all the names it is using, and Loaded (openGL)mesh/(openGL)textures/(software)collisions.
	- For Different MeshSpec object: 
		1. Each will have one Unique(Non-Shared) Renderable3DMesh(i.e. we do not gurantee a uniqueness between different MeshSpecs), i.e. The Same Mesh File can be synthesized into different renderables(e.g. making use of different materials)
		2. Each Will have one Unique(Non-Shared) Material(i.e. the parameters of a material differ a lot)
		3. Each Will have one Non-Unique(Shared) Collision Object, i.e. The same mesh file if used for collision will be shared between MeshSpec objects
		4. Point 3 isn't implemented for now
		5. Each might be referenced by either one, or many, or none MeshNodes at a time - This determined whether or not a MeshSpec will got saved when the application closes
		6. Each might be AWAKE, or ASLEEP, this has nothing to do with saving but runtime resource allocation
*/