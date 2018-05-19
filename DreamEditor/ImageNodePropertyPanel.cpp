#include "ImageNodePropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"
#include "Colors.h"

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
	labelImage = new TextLabel((unsigned short*)Interface_PropertyPanel_ImageNode, false, Color::DimDarkTextColor, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, Color::DimDarkTextColor, this);
	labelCreationTime = new TextLabel(NULL, true, Color::DimDarkTextColor, this);
	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, Color::DimDarkTextColor, this);
	labelEditTime = new TextLabel(NULL, true, Color::DimDarkTextColor, this);
	//labelComment = new TextLabel((unsigned short*)Interface_PropertyPanel_Comment, false, this);
	//textComment = new TextArea(160, 80, NULL, 0, this);	// A blank textField upon creation

	exportButton = new Button((unsigned short*)Interface_PropertyPanel_Export, this);

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(labelImage);
	layout.AddToNewRow(labelCreation);
	layout.AddToRow(labelCreationTime);
	layout.AddToNewRow(labelEdit);
	layout.AddToRow(labelEditTime);
	layout.AddToNewRow(exportButton);
	//layout.AddToNewRow(labelComment);
	//layout.AddToNewRow(textComment);
	layout.Dispatch();

	// Interface Property
	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	// Add Event Handling
	GraphManager::RegisterPropertyPanelElements(exportButton);
	//GraphManager::RegisterPropertyPanelElements(textComment);
}

ImageNodePropertyPanel::~ImageNodePropertyPanel()
{
	delete labelImage;
	delete labelCreation;
	delete labelCreationTime;
	delete labelEdit;
	delete labelEditTime;
	delete exportButton;
	//delete labelComment;
	//delete textComment;
	// I love Tab and and IDE...
}

// Interface Implementations
// Render Request
void ImageNodePropertyPanel::Render()
{
	PropertyPanel::Render();

	//labelImage->Render();
	labelCreation->Render();
	labelCreationTime->Render();
	//labelEdit->Render();
	//labelEditTime->Render();
	exportButton->Render();
	//labelComment->Render();
	//textComment->Render();
}

void ImageNodePropertyPanel::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == exportButton)
	{
		currentAssociation->Export();
	}
}

//// Interface
//void ImageNodePropertyPanel::OnElementDropWindow(int count, const char** paths)	// Might want to implement some textField drop path string
//{
//
//}

// Node Association
//void ImageNodePropertyPanel::AssociateNode(ImageNode* in)
//{	
//	// Upon association we want to update transform infor immediately and also check whetehr or not there is any comment we can show off
//
//	// Comment
//	// textComment->UpdateContent(currentAssociation->GetCommentData());
//}

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