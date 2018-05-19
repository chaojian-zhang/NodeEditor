#pragma once
#include "CanvasNode.h"
#include "Renderable3DMesh.h"
#include "MeshSpec.h"

class Terrain :public CanvasNode
{
public:
	// Constructors
	Terrain();
	~Terrain();

	// Interface
	// Not Used
	virtual void SaveData(FILE* f){};
	virtual void WakeUp(){};
	virtual void FallAsleep(){};
	// Override Base render
	virtual void Render();

private:
	// Data
	MeshSpec* meshSpec;
	// Renderable
	Renderable3DMesh* terrain;

	glm::mat4x4 world;
};