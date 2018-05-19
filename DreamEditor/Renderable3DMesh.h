#pragma once
#include "MathElegent.h"
#include "Material.h"
#include "RenderableBase.h"

// A Renderable3D Mesh is an abstract of an OpenGL object draw call to represent an rendered mesh
// A single Renderble3D Mesh might be used by multiple MeshNodes
class Renderable3DMesh
{
public:
	// Location in meters
	Renderable3DMesh(void* vertexData, unsigned int vertexSize, void* indexData, unsigned int indexSize, void* texcoordData, unsigned int texcoordSize
		, Material* inMaterial);	// Simple Shaders
	Renderable3DMesh(void* vertexData, unsigned int vertexSize, void* normalData, void* tangetData,
		void* indexData, unsigned int indexSize, void* texcoordData, unsigned int texcoordSize, Material* inMaterial);	// Phone Shader
	~Renderable3DMesh();	// Release OpenGL Objects

	void Render(glm::mat4& world, glm::mat4& view, glm::mat4& projection);	// SetUp States Using The Material and Vertex Information, Then Render

private:
	// Mesh Vertex Information
	GLuint VAO;
	GLuint buffers[5];
	unsigned int indexCount;

	// Material Information
	Material* material = NULL;

	// Bookeeping
	short bufferUsed;
};

