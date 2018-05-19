#include "MeshNodePropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"
#include "Constants.h"
#include "Colors.h"
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
	labelMesh = new TextLabel((unsigned short*)Interface_PropertyPanel_MeshNode, false, Color::DimDarkTextColor, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, Color::DimDarkTextColor, this);
	labelCreationTime = new TextLabel(NULL, true, Color::DimDarkTextColor, this);

	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, Color::DimDarkTextColor, this);
	labelEditTime = new TextLabel(NULL, true, Color::DimDarkTextColor, this);

	labelMeshName = new TextLabel((unsigned short*)Interface_PropertyPanel_MeshName, false, Color::DimDarkTextColor, this);
	labelMeshNameValue = new TextLabel(NULL, true, Color::DimDarkTextColor, this);

	labelComment = new TextLabel((unsigned short*)Interface_PropertyPanel_Comment, false, Color::DimDarkTextColor, this);
	//textComment = new TextArea(230, 80, NULL, 0, this);
	// Debug
	wchar_t* temp_string = L"This is a very very long string which 还含有中文字符哦。有没有很长长长长长长长长长长长长长长长嗄。只有足够长才能够测试TextArea的性能哦。";
	textComment = new TextArea(230, 80, (unsigned short*)temp_string, GetStringLength((unsigned short*)temp_string), this, true);

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

	//labelMesh->Render();
	labelCreation->Render();
	labelCreationTime->Render();
	//labelEdit->Render();
	//labelEditTime->Render();

	labelMeshName->Render();
	labelMeshNameValue->Render();
	labelComment->Render();
	textComment->Render();
}

// Node Association
//void MeshNodePropertyPanel::AssociateNode(MeshNode* in)
//{
//	// Upon association we want to update transform infor immediately and also check whetehr or not there is any comment we can show off
//
//	// Update Text Comment
//	// textComment->UpdateContent(currentAssociation->GetCommentData());
//}

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