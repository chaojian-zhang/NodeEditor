#pragma once

#include "CanvasNode.h"
class Canvas;

class CameraNode : public CanvasNode
{
public:
	// Object Construction
	CameraNode();	// Give it a default world location, used for loading blank DreamMap files
	// CameraNode(float x, float y, float z, glm::mat4x4* rotation);	// Initial location; Notice here we DOES have a fixed z location; Used for default creation, but not as "default as the above one; I found thid could be useful for LandMark cameras
	CameraNode(FILE* file);	// Used for loading

	// INterface Initialization
	virtual void Render(){}
	virtual void OnClick(int button, int action, int mods, float rx, float ry){};	// Not used
	virtual void OnKey(int key, int mod){};	// Camera Navigation is done by canvas
	virtual void OnChar(unsigned short c){};	// NU(Not Used)
	virtual void OnMouse(int button, int action, int mods, float rx, float ry){}
	// Serialization
	virtual void SaveData(FILE* f);
	// Dynamic Streaming
	virtual void WakeUp(){};	// NU
	virtual void FallAsleep(){};	// NU

	// Overwrite Rotate
	virtual void Rotate(float rotdx, float rotdy, float rotdz);
	virtual void Rotation(float x, float y, float z);
	// Used By Canvas
	void UpdateMatrix();	// Update View and Project Matrix

	// Utility, used by Canvas
	float GetRotationY(){ return Ry; }

public:
	// Configurations
	static float CameraHeight;
	
public:
	// Camera Node will keep a global View Matrix for every other node
	glm::mat4x4 view, project, rotation;	// Camera Doesn't necessarily need a rotation matrix because there is no gimbol lock in this case
	Canvas* canvas; // quick access
};