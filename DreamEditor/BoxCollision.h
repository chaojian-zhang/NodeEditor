#pragma once
#include "MathElegent.h"

class MeshCollision
{
public:
	MeshCollision(void* collisionMeshData, void* collissionMeshIndex);
	~MeshCollision();	// Deallocate data
	bool DetectRayCollision(Vector3f rayOrigin, Vector3f rayDirection);	// In world Space
private:
	void * collisionMesh;
	void * meshIndex;
};