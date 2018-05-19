#pragma once
#include "PropertyPanel.h"
#include "TextLabel.h"
#include "TextArea.h"
#include "Panel.h"

class MeshNode;

class MeshNodePropertyPanel : public PropertyPanel
{
public:
	MeshNodePropertyPanel();	// Initialize Interface Elements
	~MeshNodePropertyPanel();

	// Interface
	// Render Request
	virtual void Render();

	// Node Association
	// void AssociateNode(MeshNode* in);
	void Show(MeshNode* node);
	void UpdateTransformInfo();	// When an meshNode has been moved it will call this function

private:
	static float TextureCoords[8];

private:
	// Interface Elmeents
	TextLabel* labelMesh;	// "MeshNode:"
	TextLabel* labelCreation;	// "Creation Time:"
	TextLabel* labelCreationTime;	// "2015-08-30"
	TextLabel* labelEdit;	// "Edit Time"
	TextLabel* labelEditTime;	// "2015-08-30"

	TextLabel* labelMeshName;	// "Mesh Name:"
	TextLabel* labelMeshNameValue;	// "A lovely beautiful box"

	TextLabel* labelComment;	// "Commen:"
	// TextField* textComment;
	TextArea* textComment;

	// Important Referenced Object
	MeshNode* currentAssociation;
};