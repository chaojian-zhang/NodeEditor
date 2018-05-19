#pragma once
#include "MeshSpec.h"
#include "LinkedList.h"

class MeshResource
{
public:
	~MeshResource();	// Destory all MeshSpec

	void AddMesh(MeshSpec* mesh);	// Used when loading
	void Save(FILE* file);	// Find those whose reference count isn't 0, Reorder and Save

	MeshSpec* GetMesh(unsigned int index);	// Used for loading

private:
	LinkedList meshes;
};