#include "MeshNodePropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"
#include "Constants.h"
// Debug
#include "MathElegent.h"

float MeshNodePropertyPanel::TextureCoords[8] = {
	1024.0f / 2048.0f, 192.0f / 2048.0f,
	1535.0f / 2048.0f, 192.0f / 2048.0f,
	1535.0f / 2048.0f, 992.0f / 2048.f,
	1024.0f / 2048.0f, 992.0f / 2048.f
};

MeshNodePropertyPanel::MeshNodePropertyPanel()
	:PropertyPanel(TextureCoords)
{
	// Interface Obejcts
	labelMesh = new TextLabel((unsigned short*)Interface_PropertyPanel_MeshNode, false, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, this);
	labelCreationTime = new TextLabel(NULL, true, this);

	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, this);
	labelEditTime = new TextLabel(NULL, true, this);

	labelMeshName = new TextLabel((unsigned short*)Interface_PropertyPanel_MeshName, false, this);
	labelMeshNameValue = new TextLabel(NULL, true, this);

	labelComment = new TextLabel((unsigned short*)Interface_PropertyPanel_Comment, false, this);
	textComment = new TextArea(230, 80, NULL, 0, this, true);

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);

	layout.AddToNewRow(labelMesh);

	layout.AddToNewRow(labelCreation);
	layout.AddToRow(labelCreationTime);

	layout.AddToNewRow(labelMeshName);
	layout.AddToRow(labelMeshNameValue);

	layout.AddToNewRow(labelEdit);
	layout.AddToRow(labelEditTime);

	layout.AddToNewRow(labelComment);
	layout.AddToNewRow(textComment);

	layout.Dispatch();

	// Interface Property
	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	// Add Event Handling
	GraphManager::RegisterPropertyPanelElements(textComment);
}

MeshNodePropertyPanel::~MeshNodePropertyPanel()
{
	delete labelMesh;
	delete labelCreation;
	delete labelCreationTime;
	delete labelEdit;
	delete labelEditTime;
	delete labelMeshName;
	delete labelMeshNameValue;
	delete labelComment;
	delete textComment;
}

// Interface Implementations
// Render Request
void MeshNodePropertyPanel::Render()
{
	PropertyPanel::Render();

	labelCreation->Render();
	labelCreationTime->Render();

	labelMeshName->Render();
	labelMeshNameValue->Render();
	labelComment->Render();
	textComment->Render();
}

void MeshNodePropertyPanel::Show(MeshNode* node)
{
	PropertyPanel::Show();

	labelCreationTime->UpdateContent(node->GetCreateTimeString());
	labelMeshNameValue->UpdateContent(node->GetName());
	textComment->AssociateStringData(node->GetCommentData());
}

void MeshNodePropertyPanel::UpdateTransformInfo()	// When an imagenode has been moved it will call this function
{

}