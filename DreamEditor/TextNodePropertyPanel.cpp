#include "TextNodePropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"

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
	labelText = new TextLabel((unsigned short*)Interface_PropertyPanel_TextNode, false, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, this);
	labelCreationTime = new TextLabel(NULL, true, this);
	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, this);
	labelEditTime = new TextLabel(NULL, true, this);

	exportButton = new Button((unsigned short*)Interface_PropertyPanel_Export, this);

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(labelText);

	layout.AddToNewRow(labelCreation);
	layout.AddToRow(labelCreationTime);

	layout.AddToNewRow(labelEdit);
	layout.AddToRow(labelEditTime);

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
}

// Interface Implementations
// Render Request
void TextNodePropertyPanel::Render()
{
	// Remember to render background image even though its storage is not managed by us
	// backgroundImage->Render();
	// Yes, that was indeed awkward, that is why we shouldn't do that
	PropertyPanel::Render(); // Now it is better

	labelCreation->Render();
	labelCreationTime->Render();
	exportButton->Render();
}

void TextNodePropertyPanel::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == exportButton)
	{
		currentAssociation->Export();
	}
}

void TextNodePropertyPanel::Show(TextNode* node)
{
	PropertyPanel::Show();

	currentAssociation = node;
	labelCreationTime->UpdateContent(node->GetCreateTimeString());
}