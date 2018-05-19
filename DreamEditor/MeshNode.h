#pragma once
#include "CanvasNode.h"
#include "MeshSpec.h"
#include "StringData.h"
#include "Renderable3DMesh.h"

class MeshNode : public CanvasNode
{
public:
	// Constructors
	MeshNode(FILE* file);	// Load-time
	MeshNode(float x, float y, float z, const char* specFile);	// Obviously if the user drag and drop to create more nodes, things will get copied; The correct way is to make a copy when a meshNode has already been created, just like ImageNode
	~MeshNode();	// When a Mesh Node is deleted, it must notify unuse node

	// Interface
	// Serialization
	virtual void SaveData(FILE* f); // Save Index and comment
	// Dynamic Streaming
	virtual void WakeUp();	// Create OpenGL Renderables: first fetch/request data from MeshData, and if we didn't get what we want, go sleep again until next frame, try again
	virtual void FallAsleep();	// Destroy OpenGL Renderables, but still keep reference to the data
	// Override Base render
	virtual void Render();
	// Overrride Parent Translation
	virtual void Translation(float x, float y, float z);
	virtual void Rotation(float x, float y, float z);
	virtual void Scaling(float s);
	virtual void Translate(float dx, float dy, float dz);
	virtual void Rotate(float rotdx, float rotdy, float rotdz);
	virtual void Scale(float ds);

	// Interaction Interface
	virtual void OnMouseOver(double xpos, double ypos);
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos);
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods);
	virtual void OnMouseLeave();

	// Facilities
	unsigned short* GetCreateTimeString(){ return createTime.GetString(); };
	StringData* GetCommentData(){ return commentData; }
	unsigned short* GetName();
	bool RayCollision(glm::vec3 rayDirWorld, glm::vec3 rayLocWorld);
	Programs GetProgram() { return meshSpec->GetProgram(); }
	float GetSize() { return meshSpec->GetRadius(); };

private:
	// Async Loading: We will not be able to wake up, untill we fetched the Data we need

	// Data
	Time createTime;
	StringData* commentData;
	MeshSpec* meshSpec = NULL;

	// Notice we do not own the renderable, here is just a renference
	Renderable3DMesh* renderMesh = NULL;

	// Translation Details
	void UpdateMatrix();
	glm::mat4x4 rotation;
	glm::mat4x4 world;
};