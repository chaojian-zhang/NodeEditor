#pragma once
#include "PropertyPanel.h"
#include "Button.h"
#include "TextLabel.h"
#include "TextArea.h"
#include "Panel.h"

// Avoid Circular Dependancy
class ImageNode;

class ImageNodePropertyPanel : public PropertyPanel
{
public:
	ImageNodePropertyPanel();	// Initialize Interface Elements
	~ImageNodePropertyPanel();

	// Interface
	// Render Request
	virtual void Render();

	// SPecific Image Node Function
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);	// Action handling: Call ImageNode Export

	// Node Association
	// void AssociateNode(ImageNode* in);
	void Show(ImageNode* node);
	void UpdateTransformInfo();	// When an imagenode has been moved it will call this function

private:
	static float TextureCoords[8];

private:
	// Interface Elmeents
	TextLabel* labelImage;	// "Image:"
	TextLabel* labelCreation;	// "Creation Time:"
	TextLabel* labelCreationTime;	// "2015-08-30"
	TextLabel* labelEdit;	// "Edit Time"
	TextLabel* labelEditTime;	// "2015-08-30"

	Button* exportButton;

	//TextLabel* labelComment;
	//TextArea* textComment;

	// Important Referenced Object
	ImageNode* currentAssociation;
};