#pragma once
#include "PropertyPanel.h"
#include "TextLabel.h"
#include "Panel.h"
#include "Button.h"

class GraphManager;

/* Property panel is responsible for displaying and editing the properties of: Graph Document itself, and The nodes in Canvas;
	- For the nodes in canvas, some properties, e.g. Transformation and Texts, will be directly editable interacting with the canvas and node themselves,
	some properties, are only viewable and editable from inside the property panels. Others are viewable and maybe interactble in both property panel and ndoe themselves,
	but may have different presentations.
*/

class DocumentPropertyPanel : public PropertyPanel
{
public:
	DocumentPropertyPanel(GraphManager* g);	// Initialize Its elements
	~DocumentPropertyPanel();	// Destroy its elements

	// INterface 
	// Render Request
	virtual void Render();
	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);	// Action handling: Call GraphManager export function

private:
	static float TextureCoords[8];

private:
	// Interface Elements
	TextLabel* labelDocument;	// "Document:"
	TextLabel* labelCreation;	// "Creation Time:"
	TextLabel* labelCreationTime;	// "2015-08-30"
	TextLabel* labelEdit;	// "Edit Time"
	TextLabel* labelEditTime;	// "2015-08-30"

	// If we really need Pan functionality, then we need some variables like interfaceElementsWidth, and interfaceElmentsHeight, which might differ from viewport width and height so that we could pan things.
	// However, I highly doubt whether it is necessary to do that. Since we might not even need the ability to resize property panels

	Button* exportButton;

	// Important Reference Object
	GraphManager* graph;
};