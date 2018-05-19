#include "DocumentPropertyPanel.h"
#include "GridLayout.h"
#include "GraphManager.h"
#include "Colors.h"

float DocumentPropertyPanel::TextureCoords[8] = {
	1536.0f / 2048.0f, 192.0f / 2048.0f,
	2047.0f / 2048.0f, 192.0f / 2048.0f,
	2047.0f / 2048.0f, 992.0f / 2048.f,
	1536.0f / 2048.0f, 992.0f / 2048.f
};

DocumentPropertyPanel::DocumentPropertyPanel(GraphManager* g)
	:PropertyPanel(TextureCoords), graph(g)
{
	// Interface Obejcts
	labelDocument = new TextLabel((unsigned short*)Interface_PropertyPanel_Document, false, Color::DimDarkTextColor, this);
	labelCreation = new TextLabel((unsigned short*)Interface_PropertyPanel_CreationTime, false, Color::DimDarkTextColor, this);
	labelCreationTime = new TextLabel(graph->createTime.GetString(), false, Color::DimDarkTextColor, this);
	labelEdit = new TextLabel((unsigned short*)Interface_PropertyPanel_EditTime, false, Color::DimDarkTextColor, this);
	labelEditTime = new TextLabel(graph->modifyTime.GetString(), false, Color::DimDarkTextColor, this);

	exportButton = new Button((unsigned short*)Interface_PropertyPanel_Export, this);

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(labelDocument);
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

DocumentPropertyPanel::~DocumentPropertyPanel()
{
	delete labelDocument;
	delete labelCreation;
	delete labelCreationTime;
	delete labelEdit;
	delete labelEditTime;
	delete exportButton;
}

// Interface Implementation
void DocumentPropertyPanel::Render()
{
	PropertyPanel::Render();

	//labelDocument->Render();
	labelCreation->Render();
	labelCreationTime->Render();
	labelEdit->Render();
	labelEditTime->Render();
	exportButton->Render();
}

void DocumentPropertyPanel::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == exportButton)
	{
		GraphManager::canvas->ExportAll();
	}
}

// __Pending Deprecation
//void DocumentPropertyPanel::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
//{
//	// Pan the interface ELements: When Property channel is panned, it will be responsible for updating all its interface elements
//}