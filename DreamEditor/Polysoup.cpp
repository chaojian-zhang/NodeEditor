#include "Polysoup.h"
#include <stdlib.h>

// __Debug__: Pending a Debug Renderable for Visualization purpose

Polysoup::Polysoup(unsigned int inIndexCount, unsigned int* inIndices, unsigned int inNumVerts, float* inVerts, bool simpleBounding)
{
	// Initialize Sampling Vertices
	XYZ = nXYZ = glm::vec3(inVerts[0], inVerts[1], inVerts[2]);

	// Iterate and calculate collision boundary for fast distance check
	// Also find out numerical center, too keep accuracy we calculate new average every new vert
	for (unsigned int i = 0; i < inNumVerts; i++)	// Per Vertex
	{
		// Update XYZ
		if (inVerts[i * 3 + 0] >= XYZ.x) XYZ.x = inVerts[i * 3 + 0];
		if (inVerts[i * 3 + 1] >= XYZ.y) XYZ.y = inVerts[i * 3 + 1];
		if (inVerts[i * 3 + 2] >= XYZ.z) XYZ.z = inVerts[i * 3 + 2];
		
		// Update nXYZ
		if (inVerts[i * 3 + 0] <= nXYZ.x)	nXYZ.x = inVerts[i * 3 + 0];
		if (inVerts[i * 3 + 1] <= nXYZ.y)	nXYZ.y = inVerts[i * 3 + 1];
		if (inVerts[i * 3 + 2] <= nXYZ.z)	nXYZ.z = inVerts[i * 3 + 2];

		// Update Center
		center = center + glm::vec3((inVerts[i * 3 + 0] - center.x) / (i+1), 
			(inVerts[i * 3 + 1] - center.y) / (i+1), 
			(inVerts[i * 3 + 2] - center.z) / (i+1));
	}

	// Save Data
	if (simpleBounding)
	{
		// Simply set to NULL, rather than construct a cube
		indices = NULL;
		verts = NULL;
	}
	else
	{
		indexCount = inIndexCount;
		unsigned int iSize = indexCount * sizeof(unsigned int);
		indices = (unsigned int*)malloc(iSize);
		memcpy(indices, inIndices, iSize);

		numVerts = inNumVerts;
		unsigned int vSize = numVerts * sizeof(float) * 3;	// Ech vert occupies 3 floats
		verts = (float*)malloc(vSize);
		memcpy(verts, inVerts, vSize);
	}
}

Polysoup::~Polysoup()
{
	// Delete data
	delete indices;
	delete verts;
}

// FOr mysterious reasons, it is much safer to make it a seperate function and doesn't change anything.
static bool TriangleIntersect(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& rayDir, const glm::vec3& rayOrigin)
{
	// test ray direction against triangle
	glm::vec3 e1 = v1 - v0;
	glm::vec3 e2 = v2 - v0;
	glm::vec3 p = glm::cross(rayDir, e2);
	float a = glm::dot(e1, p);
	// if result zero, no intersection or infinite intersections
	// (ray parallel to triangle plane)
	if (a > -0.00001 && a < 0.00001)
		return false;
	// compute denominator
	float f = 1.0f / a;
	// compute barycentric coordinates
	glm::vec3 s = rayOrigin - v0;
	float u = f * glm::dot(s, p);
	if (u < 0.0f || u > 1.0f) return false;
	glm::vec3 q = glm::cross(s, e1);
	float v = f * glm::dot(rayDir, q);
	if (v < 0.0f || u + v > 1.0f) return false;
	// compute line parameter
	float t = f * glm::dot(e2, q);
	return (t >= 0);
}

bool Polysoup::CheckRayCollision(const glm::vec3& rayDir, const glm::vec3& rayLoc)
{
	// If Use only simple bounds
	if (indices == NULL || verts == NULL)
	{
		// Do Spherical Check
		float radius = glm::length(XYZ - nXYZ) / 2.82;	// 2*sqrt(2) to get a more accurate radius
		// Get Distance From ray to center of object
		glm::vec3 Q_S = center - rayLoc;	// Temp Parameter: Direction to the rayLoc
		float QSDotV = glm::dot(Q_S, rayDir);	// Temp Parameter
		return glm::sqrt(glm::dot(Q_S, Q_S) - QSDotV * QSDotV) <= radius;
	}

	// Otherwise do poly soup check
	// Iterate through each triangle and do collision detection
	for (unsigned int i = 0; i < indexCount; i += 3)	// Threee Indexes Per triangular face
	{
		// Source See Essential Mathematics p587
		// Fetch the points
		unsigned int index0 = indices[(i + 0)];
		unsigned int index1 = indices[(i + 1)];
		unsigned int index2 = indices[(i + 2)];
		glm::vec3 P1 = glm::vec3(verts[index0 * 3 + 0], verts[index0 * 3 + 1], verts[index0 * 3 + 2]);
		glm::vec3 P2 = glm::vec3(verts[index1 * 3 + 0], verts[index1 * 3 + 1], verts[index1 * 3 + 2]);
		glm::vec3 P3 = glm::vec3(verts[index2 * 3 + 0], verts[index2 * 3 + 1], verts[index2 * 3 + 2]);

		// bool TriangleIntersect(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& rayDir, glm::vec3& rayOrigin)
		if (TriangleIntersect(P1, P2, P3, rayDir, rayLoc)) return true;

		//// Get the vectors
		////glm::vec3 V1 = glm::normalize(P2 - P1);
		////glm::vec3 V2 = glm::normalize(P3 - P1);	// Notice it should be relative to P1, just as V1
		//glm::vec3 V1 = P2 - P1;
		//glm::vec3 V2 = P3 - P1;	// Notice it should be relative to P1, just as V1
		//// Note: Normalization seems the biggest difference between mine and the original code

		//// u,v for bicentric coord inside triangle, and t for the ray travelling paramter of ray function

		//// Check Whether Ray is Parallel to the Plane of the Traingle
		//// Test ray direction against triangle
		//glm::vec3 h = glm::cross(rayDir, V2);	// The Cross product betweeen Ray and one Edge of the Triangle
		//float a = glm::dot(V1, h);
		//// if result zero, no intersection or infinite intersections
		//// (ray parallel to triangle plane)
		//if (a > -0.00001 && a < 0.00001)
		//	continue;

		//// compute denominator
		//float f = 1.f / a;

		//// Compute Barycentric Coordinates
		//glm::vec3 s = rayLoc - P1;
		//float u = f * (glm::dot(s, h));
		//if (u < 0.0 || u > 1.0)
		//	continue;

		//glm::vec3 q = glm::cross(s, V1);
		//float v = f * glm::dot(rayDir, q);
		//if (v < 0.0 || u + v > 1.0)
		//	continue;

		//// At this stage we can compute t to find out where
		//// the intersection point is on the line
		//float t = f * glm::dot(V2, q);

		//if (t > 0.00001) // ray intersection
		//	return true;

		//else // this means that there is a line intersection
		//	// but not a ray intersection
		//	continue;
	}

	return false;
}

float Polysoup::ApproxNearestDistanceTo(const glm::vec3& location)
{
	// Make it a Sphere to further simplify calculation
	float radius = glm::length(XYZ - nXYZ) / 2.82;

	// Calculate the distance from location to sphere surface
	return glm::length(location - center) - radius;
}

float Polysoup::GetDomainRadius()
{
	return glm::length(XYZ - nXYZ) / 2.82;
}