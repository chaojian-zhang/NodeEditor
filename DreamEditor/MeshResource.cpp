#include "MeshResource.h"
#include "GlobalMacros.h"

MeshResource::~MeshResource()
{
	MeshSpec* mesh = (MeshSpec*)meshes.First();
	while (mesh)
	{
		delete mesh;
		mesh = (MeshSpec*)meshes.Next();
	}
}

void MeshResource::AddMesh(MeshSpec* mesh)
{
	meshes.Add(mesh);
}

void MeshResource::Save(FILE* file)
{
	MeshSpec* mesh = (MeshSpec*)meshes.First();
	unsigned short savingSlot = 0;	// savingSlot indexing
	while (mesh)
	{
		if (mesh->GetReferenceCount() > 0)
		{
			// Tag a Block
			unsigned char tag = MESH_DATA_BLOCK_TAG;
			fwrite(&tag, sizeof(unsigned char), 1, file);

			// Save Filename Length
			const char* fileName = mesh->GetSpecFileLocation();
			unsigned int nameLength = strlen(fileName);
			fwrite(&nameLength, sizeof(unsigned int), 1, file);

			// Save Filename
			fwrite(fileName, nameLength, 1, file);

			// Record the index of saving slot
			mesh->SetSavingSlot(savingSlot);

			// Increament saving slot if we saved a mesh resource
			savingSlot++;
		}

		mesh = (MeshSpec*)meshes.Next();
	}
}

MeshSpec* MeshResource::GetMesh(unsigned int index)
{
	return (MeshSpec*)(meshes.Select(index));
}