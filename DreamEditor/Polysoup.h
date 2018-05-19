#pragma once
#include <glm\matrix.hpp>
#include <glm\mat4x4.hpp>

class Polysoup
{
public:
	// Expect Triangulated Faces
	// Specify SimpleBounding to indicate that input verts isn't used for bounding, but for construction only
	// DO NOT create any rendering related stuff during constructor because it is created in a working thread
	Polysoup(unsigned int inIndexCount, unsigned int* inIndices, unsigned int inNumVerts, float* inVerts, bool simpleBounding = false);
	~Polysoup();

	// Check Collision using ray in Object Space
	// @rayDir should be normalized
	bool CheckRayCollision(const glm::vec3& rayDir, const glm::vec3& rayLoc);

	// Check Closest Distance to location, in Object Space; Can be used to check distance to camera
	// @location should be in object space
	float ApproxNearestDistanceTo(const glm::vec3& location);
	float GetDomainRadius();

private:
	unsigned int indexCount;	// numFaces = indexCount / 3;
	unsigned int* indices;	// Dynamically allocated

	unsigned int numVerts;	
	float* verts;	// Dynamically allocated

	// Simple Box Boundary, rotate and translate with world matrix.
	glm::vec3 XYZ;
	glm::vec3 nXYZ;
	glm::vec3 center;
};