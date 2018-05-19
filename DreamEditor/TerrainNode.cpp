#include "TerrainNode.h"
#include "SystemWrapper.h"
#include "Constants.h"
#include "GraphManager.h"

Terrain::Terrain()
{
	// Create a mesh data
	meshSpec = new MeshSpec(ApplicationPath::TerrainMeshSpecMath);
	meshSpec->RequireReference();

	// Try to wake up the mesh spec
	terrain = meshSpec->RequireMeshUsage();
	while (!terrain)
	{
		terrain = meshSpec->RequireMeshUsage();
	}

	// Update Matrix
	world = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
}

Terrain::~Terrain()
{
	meshSpec->ReleaseMeshUsage();
	meshSpec->ReleaseReference();
}

void Terrain::Render()
{
	// Render the mesh
	Canvas* canvas = GraphManager::canvas;
	terrain->Render(world, canvas->GetViewMatrix(), canvas->GetProjectMatrix());
}
