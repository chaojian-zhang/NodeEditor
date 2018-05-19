#pragma once
#include "LinkedList.h"
#include "PopupMenu.h"
#include "Canvas.h"
#include "MeshResource.h"
#include "TextLabel.h"
#include "Button.h"
#include "StaticShape.h"
class GraphManager;

class MeshCreationPopupMenu : public PopupMenu
{
public:
	MeshCreationPopupMenu(GraphManager* g);	// Set Up all Elements
	~MeshCreationPopupMenu();	// Destroy all Elements

	// Render
	virtual void Render();
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);

private:
	// Interface Elements
	TextLabel* addMeshLabel;	// "Add MeshNode"
	Button* rotatingCube;
	Button* anywhereDoor;	// To be differentiated from "dimension door", which leads to a pocket dimension, not another part of the world
	StaticShape* backgroundRect;

private:
	// Use a linked list to help managing
	LinkedList buttons;

	// Important Referenced Object
	GraphManager* graph;
};