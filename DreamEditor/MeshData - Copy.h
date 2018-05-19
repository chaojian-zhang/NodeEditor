#pragma once
#include "Shaders.h"
#include "Renderable3DMesh.h"
#include <stdio.h>
#include "MathElegent.h"
#include "SystemWrapper.h"	// FOr string Conversoin

/* Provides description about the mesh in use, the actual mesh file is stored seperately on disk*/
class  MeshData
{
public:
	MeshData(char* meshSpecFilename);	// Used for Dyna Drag-n-drop loading
	MeshData(FILE* file);	// Used for loading
	~MeshData();

	void Save(FILE* file);	// Save index, mesh spec filename size, and filename

	// Basic Information Query: Those are alrady in memory

	// Reqeuets: OpenGL Object Creation
	Renderable3DMesh* GetMesh();	// Originally we have no data available, i.e. mesh == false. In this case we return NULL, and MeshNode who tried to get data will get nothing, so he will not 
	// be able to wake up, so he just continues sleep, untill next frame try again.

	// MeshNode will call the below functions to keep MeshData's usageCount balanced
	void UseMesh();
	void UnuseMesh();

public:
	unsigned short* meshFileNameU16;
	float iconCoord[8];	// Icon vertex coordinates of the icon file
	GLuint iconTexture = 0;

private:
	// MeshSpec Loading Logic
	void Load(char* filename);
	// Mesh Streaming Logic
	static void Stream(MeshData* meshData);	// It gotta to be static so that we can create a thread of it
	// Caution: Even though this function is static, we should not make any member variable of MeshData static, because they are NOT

private:
	// Loading Time-Data
	char* meshSpecFilename = NULL;	// ANSI terminate with null
	char* meshFileName = NULL;	// 3D mesh file(i.e. .obj file) name
	char* textureFileName= NULL;	// PNG file name
	Vector3f pointLightPos;
	char* collisionName = NULL;	// 3D mesh file(i.e. .obj file) name
	char* iconMapName = NULL;
	Programs program;	// Shader Program of choice

	// Streaming Time-Data
	bool bAlreadyLoading = false;
	Renderable3DMesh* mesh = NULL;	// Only Create when needed

	// Bookkeeping
	unsigned int usageCount = 0;	// Mesh data might not be referenced by anyone because popup menu will just load them all
};

// Notes: 
// If the texture cannot be found at runtime, the best thing we could do is to render a red Mark, pending providing that feature