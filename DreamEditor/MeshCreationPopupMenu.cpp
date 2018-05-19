#include "MeshCreationPopupMenu.h"
#include "GridLayout.h"
#include "GraphManager.h"
#include "Colors.h"

MeshCreationPopupMenu::MeshCreationPopupMenu(GraphManager* g)
	:graph(g)
{
	// Create Interface Elements
	// A Label that indicates the purpose of the Screen
	addMeshLabel = new TextLabel((unsigned short*)Interface_MeshCreationPop_AddMesh, false, Color::LightGreyTextColor, this);
	// A Cube Mesh
	rotatingCube = new Button((unsigned short*)Interface_MeshCreationPop_RotatingCube, this, MenuButton);
	buttons.Add(rotatingCube);
	// An Anywhere Door
	anywhereDoor = new Button((unsigned short*)Interface_MeshCreationPop_AnywhereDoor, this, MenuButton);
	buttons.Add(anywhereDoor);

	// Use a layout
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);
	layout.AddToNewRow(addMeshLabel);
	// Iterate and add lines
	Button* temp = (Button*)(buttons.First());
	while (temp)
	{
		layout.AddToNewRow(temp);
		temp = (Button*)(buttons.Next());
	}
	// Do a Dispatch
	layout.Dispatch();

	// Set up correct viewport size
	viewportW = layout.layoutWidth + LayoutBeginPositionX;
	viewportH = layout.layoutHeight - LayoutBeginPositionY;

	// Set up a background shape
	backgroundRect = new StaticShape(this, ShapeType::Shape_BorderedSquare, viewportW, viewportH);

	// Offset Z Height abit to avoid being overlaped by other interfaces.
	backgroundRect->OffsetPosition(0, 0, 0.2);
	addMeshLabel->OffsetPosition(0, 0, 0.4);
	// Iterate and adjust
	temp = (Button*)(buttons.First());
	while (temp)
	{
		temp->OffsetPosition(0, 0, 0.4);
		temp = (Button*)(buttons.Next());
	}

	// Setup Interaction Callbacks
	GraphManager::RegisterPopupMenuElements(rotatingCube);
	GraphManager::RegisterPopupMenuElements(anywhereDoor);

	// Update Projection and View Matrix
	UpdateProjection();
}

MeshCreationPopupMenu::~MeshCreationPopupMenu()
{
	// Delete Interface Elements
	delete addMeshLabel;
	delete backgroundRect;
	// Iterate and delete
	Button* temp = (Button*)(buttons.First());
	while (temp)
	{
		delete temp;
		temp = (Button*)(buttons.Next());
	}
}

void MeshCreationPopupMenu::Render()
{
	backgroundRect->Render();	// This must be rendered before addMeshLabel, otherwise we will see window effect
	addMeshLabel->Render();
	// Iterate and delete
	Button* temp = (Button*)(buttons.First());
	while (temp)
	{
		temp->Render();
		temp = (Button*)(buttons.Next());
	}
}

void MeshCreationPopupMenu::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
{
	if (element == rotatingCube)
	{

	}
	else if (element == anywhereDoor)
	{

	}
}