#include "TextNodePropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"
#include "Colors.h"

float TextNodePropertyPanel::TextureCoords[8] = {
	0.0f / 2048.0f, 192.0f / 2048.0f,
	511.0f / 2048.0f, 192.0f / 2048.0f,
	511.0f / 2048.0f, 992.0f / 2048.f,
	0.0f / 2048.0f, 992.0f / 2048.f
};

TextNodePropertyPanel::TextNodePropertyPanel()
	:PropertyPanel(TextureCoords)
{
	// Interface Obejcts
	labelText = new TextLabel((unsigned short*)Interface_PropertyPanel_TextNode, false, Color::DimDarkTextColor, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, Color::DimDarkTextColor, this);
	labelCreationTime = new TextLabel(NULL, true, Color::DimDarkTextColor, this);
	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, Color::DimDarkTextColor, this);
	labelEditTime = new TextLabel(NULL, true, Color::DimDarkTextColor, this);
	
	//labelTransformation = new TextLabel((unsigned short*)Interface_PropertyPanel_Transformation, false, this);
	//labelX = new TextLabel((unsigned short*)Interface_PropertyPanel_TransformationX, true, this);
	//labelY = new TextLabel((unsigned short*)Interface_PropertyPanel_TransformationY, true, this);
	//labelZ = new TextLabel((unsigned short*)Interface_PropertyPanel_TransformationZ, true, this);

	//labelXValue = new TextLabel(NULL, true, this);
	//labelYValue = new TextLabel(NULL, true, this);
	//labelZValue = new TextLabel(NULL, true, this);

	exportButton = new Button((unsigned short*)Interface_PropertyPanel_Export, this);

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(labelText);

	layout.AddToNewRow(labelCreation);
	layout.AddToRow(labelCreationTime);

	layout.AddToNewRow(labelEdit);
	layout.AddToRow(labelEditTime);

	//layout.AddToNewRow(labelTransformation);
	//layout.AddToNewRow(labelX);
	//layout.AddToRow(labelXValue);
	//layout.AddToNewRow(labelY);
	//layout.AddToRow(labelYValue);
	//layout.AddToNewRow(labelZ);
	//layout.AddToRow(labelZValue);

	layout.AddToNewRow(exportButton);

	layout.Dispatch();

	// Interface Update
	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	// Add Event Handling
	GraphManager::RegisterPropertyPanelElements(exportButton);
}

TextNodePropertyPanel::~TextNodePropertyPanel()
{
	delete labelText;
	delete labelCreation;
	delete labelCreationTime;
	delete labelEdit;
	delete labelEditTime;
	delete exportButton;
	
	// I love Tab and and IDE...
}

// Interface Implementations
// Render Request
void TextNodePropertyPanel::Render()
{
	// Remember to render background image even though its storage is not managed by us
	// backgroundImage->Render();
	// Yes, that was indeed awkward, that is why we shouldn't do that
	PropertyPanel::Render(); // Now it is better

	//labelText->Render();
	labelCreation->Render();
	labelCreationTime->Render();
	//labelEdit->Render();
	//labelEditTime->Render();
	exportButton->Render();
}

void TextNodePropertyPanel::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == exportButton)
	{
		currentAssociation->Export();
	}
}

// Node Association
//void TextNodePropertyPanel::AssociateNode(TextNode* in)
//{
//	// Upon association we want to update transform infor immediately, and also time information
//	// Upon association we might also want to show ourselves while hide other property panels, if we are not globally hidden(i.e. no distraction mode)
//}

void TextNodePropertyPanel::Show(TextNode* node)
{
	PropertyPanel::Show();

	currentAssociation = node;
	labelCreationTime->UpdateContent(node->GetCreateTimeString());
	// labelEditTime->UpdateContent(node->GetModifyTimeString());	// Not Used

	/*labelXValue->UpdateContent
	labelYValue
	labelyValue*/
}

void TextNodePropertyPanel::UpdateTransformInfo()	// When an imagenode has been moved/rotated it will call this function
{
	// Not Used
}