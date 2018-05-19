#pragma once
#include <stdio.h>
#include "LinkedList.h"
#include <glm\matrix.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class CanvasNode
{
public:
	// Serialization
	virtual void SaveData(FILE* f) = 0;
	// Dynamic Streaming
	virtual void Update(){};
	virtual void WakeUp(){ if (!bSleep){ return; }	/*In case fake wake up*/ if (bPendingDeletion){ return; } bSleep = false; };	// Create OpenGL Renderables
	virtual void FallAsleep() { if (bSleep == true){ return; }bSleep = true; };	// Destroy OpenGL Renderables, but still keep reference to the data

	// Interaction Interface
	virtual void OnMouseOver(double xpos, double ypos);	// Inputs only meaningful for Text and Image node, in which case it is relative to X, Y origin; Y is negative
	virtual void OnMouseButton(int button, int action, int mods, double xpos, double ypos){};
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods){};
	virtual void OnTextInput(unsigned short* text, unsigned int numChars){};
	virtual void OnMouseLeave(){ if (bPendingDeletion){ return; } bMouseOverNotified = false; };
	virtual void OnDeselection(){};	// Called when being deselected on Canvas, Image and Text nodes need to implement this for cursor blinking

	// Common Interaction Functionalities
	virtual void Render();	// Provide generic rendering for Image and Text Nodes; MeshNodes override this
	virtual void Translation(float x, float y, float z);	// Children implement this to provide specific initial layout
	// All below only MeshNode has its own implementation
	virtual void Rotation(float x, float y, float z);
	virtual void Scaling(float s);
	virtual void Translate(float dx, float dy, float dz);
	virtual void Rotate(float rotdx, float rotdy, float rotdz);
	virtual void Scale(float ds);

	// Used By Canvas
	glm::vec3 GetWorldPosition();

	// __Debug__: Seems working well
	// alignment
	static void* operator new (size_t size);
	static void operator delete (void *p);

public:
	// State Variables
	bool bSleep = true;	// A Node is not awake when it is too far away from being seen and still far away from potentially being seen
	// All nodes need to be manually woke up
	// Checked by Canvas to determine render or not
	bool bInvisible; // UPdated per frame by Canvas to facilitate culling
	bool bMouseOverNotified = false;

	// State Variable
	bool bPendingDeletion = false;	// Canvas node deletion management
	// Actual deletion will happen after the application is closed; Yeah this sucks, just like Photoshop

	// Property, the Only use here is for collision detection
	float width, height, depth = 0;	// For planar nodes, depth is 0

protected:
	float Tx, Ty, Tz;	// Represent location of the origin of a node
	float Rx, Ry, Rz;	// Notice this doesn't avoid gimbal lock
	float S;	// Scale

protected:
	// Canvas Node Display Renderables, used for generic interface of renderables transformation
	LinkedList nodalRenderables;	// Those except being controlled by TextContainer
	LinkedList textContainers;	// Specially handling textContainers
};