#pragma once
#include "PropertyPanel.h"
#include "Button.h"
#include "TextLabel.h"
#include "Panel.h"

class TextNode;

/* Provide Access to TextNode properties*/
/* Apply Change to TextNode: None(Transformation Information are not directly Editable since not needed, the only one that needs to be editable is for MeshNode, scale property);
	Exposed Fields of TextNode: Tranformation Information, Export Button;
*/
class TextNodePropertyPanel : public PropertyPanel
{
public:
	TextNodePropertyPanel();
	~TextNodePropertyPanel();

	// Interface Implementation
	// Render Request
	virtual void Render();

	// Specific TextNode interactions
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);	// Action handling: Call TextNode Export()

	// Node Association
	// void AssociateNode(TextNode* in); // Change focus from current associated node the newly selected one, update Interface Elements Accordingly
	void Show(TextNode* node);
	void UpdateTransformInfo();	// When an text node  has been moved it will call this function
	// Acess TextNode, get TransformData, and updata Interface Elements

private:
	static float TextureCoords[8];

private:
	// Interface Elements: Specific to interactions with TextNode
	TextLabel* labelText;	// "Image:"
	TextLabel* labelCreation;	// "Creation Time:"
	TextLabel* labelCreationTime;	// "2015-08-30"

	TextLabel* labelEdit;	// "Edit Time"
	TextLabel* labelEditTime;	// "2015-08-30"

	//// __Debug For purely experimental purpose, final release doesn't need
	//TextLabel* labelTransformation;	// "Transformation:"
	//TextLabel* labelX;	// "X"
	//TextLabel* labelY;	// "Y"
	//TextLabel* labelZ;	// "Z"

	//TextLabel* labelXValue;	// "0.1256"
	//TextLabel* labelYValue;	// "0.3578"
	//TextLabel* labelZValue;	// "0.3684"

	Button* exportButton;

	// Important Referenced Object
	TextNode* currentAssociation = NULL;
};