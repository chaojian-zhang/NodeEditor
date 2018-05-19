#include "ImageNodePropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"

float ImageNodePropertyPanel::TextureCoords[8] = {
	512.0f / 2048.0f, 192.0f / 2048.0f,
	1023.0f / 2048.0f, 192.0f / 2048.0f,
	1023.0f / 2048.0f, 992.0f / 2048.f,
	512.0f / 2048.0f, 992.0f / 2048.f
};

ImageNodePropertyPanel::ImageNodePropertyPanel()
	:PropertyPanel(TextureCoords)
{
	// Interface Obejcts
	labelImage = new TextLabel((unsigned short*)Interface_PropertyPanel_ImageNode, false, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, this);
	labelCreationTime = new TextLabel(NULL, true, this);
	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, this);
	labelEditTime = new TextLabel(NULL, true, this);

	exportButton = new Button((unsigned short*)Interface_PropertyPanel_Export, this);

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(labelImage);
	layout.AddToNewRow(labelCreation);
	layout.AddToRow(labelCreationTime);
	layout.AddToNewRow(labelEdit);
	layout.AddToRow(labelEditTime);
	layout.AddToNewRow(exportButton);
	layout.Dispatch();

	// Interface Property
	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	// Add Event Handling
	GraphManager::RegisterPropertyPanelElements(exportButton);
}

ImageNodePropertyPanel::~ImageNodePropertyPanel()
{
	delete labelImage;
	delete labelCreation;
	delete labelCreationTime;
	delete labelEdit;
	delete labelEditTime;
	delete exportButton;
}

// Interface Implementations
// Render Request
void ImageNodePropertyPanel::Render()
{
	PropertyPanel::Render();


	labelCreation->Render();
	labelCreationTime->Render();

	exportButton->Render();

}

void ImageNodePropertyPanel::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == exportButton)
	{
		currentAssociation->Export();
	}
}

void ImageNodePropertyPanel::Show(ImageNode* node)
{
	// Caller is responsible to make sure node isn't NULL or wrong type

	PropertyPanel::Show();

	currentAssociation = node;
	labelCreationTime->UpdateContent(node->GetCreateTimeString());
}

void ImageNodePropertyPanel::UpdateTransformInfo()	// When an imagenode has been moved it will call this function
{
	// Since we are not displaying any transformation information this isn't needed
}