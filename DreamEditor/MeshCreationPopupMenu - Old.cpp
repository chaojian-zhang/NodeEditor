#include "MeshCreationPopupMenu.h"
#include "GridLayout.h"

// Deprecation note: this old version seems more complicated than current active version since it tried to deal with dynamically loaded meshes, which isn't that necessary

/* Note: Management
	1. Creation: All Elements and thus renderables use relative coord relative to each other, and when The popup is summoned somewhere else it can call UpdateLocation on Interface Elements thus update the renderables
		Viewport Range is all we care
	2. Show and Hide: Popups' viewport location are determined by caller
	3. Mesh Data Synchronizaton: 
*/
// New Design requries no mesh being loaded, but just filenames + system mesh icons, and mesh creation popup menu will not dynamically change elements. When some shape is selected, we will load the meshSPec in real time.
//	 From this point of view, what being displayed on the creation popup menu can be configured using a configuratoin file
// Create All Interface Elements, layout them nicely
MeshCreationPopupMenu::MeshCreationPopupMenu(Canvas* c, MeshResource* meshResource)
	:canvas(c)
{
	// Count Slots
//	unsigned short numMeshes = meshResource->GetNumMeshes();

	// Interface Obejcts
	addMeshLabel = new TextLabel((unsigned short*)Interface_PopupMenu_AddMeshNode, false, this);
	// __Pending divisionLine = new DivisionLine();	// Use default-length division line as place holder, we will feed scale data later

	// Initialzie Button elements using a linked list
	//MeshData* mesh;
	//for (int i = 0; i < numMeshes; i++)
	//{
	//	mesh = meshResource->GetMeshData(i);
	//	buttons.Add(new Button(mesh->iconCoord, meshResource->GetMeshName(i), mesh->iconTexture, this));
	//}

	// Layout Things
	AutoExpandGridLayout layout(LayoutBeginPositionX, LayoutBeginPositionY, LayoutRowPadding, LayoutColumnPadding);	// Rememebr here the position isn't used to place the renderables globally but just relative to the layout
	layout.AddToNewRow(addMeshLabel);
	// __Pending layout.AddToNewRow(divisionLine);

	//if (buttons.Length() != 0)
	//{
	//	layout.AddToNewRow((Button*)(buttons.First()));
	//	for (int i = 1; i < numMeshes; i++)
	//	{
	//		layout.AddToNewRow((Button*)(buttons.Next()));
	//	}

	//}

	layout.Dispatch();

	// Create a background rect interface element
	// __Pending: This would be too small, might want to add some padding
	backgroundRect = new StaticShape(this, ShapeType::Shape_BorderedSquare, layout.layoutWidth, layout.layoutHeight);

	// Set Correct Viewport dimensions
	// __Pending: This would be too small, might want to add some padding
	viewportW = layout.layoutWidth;
	viewportH = layout.layoutHeight;
}

// Delete All Interface Elements
MeshCreationPopupMenu::~MeshCreationPopupMenu()
{
	delete addMeshLabel;
	// __Pending delete divisionLine;
	Button* button = (Button*)buttons.First();
	while (button)
	{
		delete button;
		button = (Button*)buttons.Next();
	}
	delete backgroundRect;
}

// Interface Implementations
// Render
void MeshCreationPopupMenu::Render()
{

}
//
//void MeshCreationPopupMenu::OnElementMouseButton(int button, int action, int mods, double xpos, double ypos)
//{
//	// Find the index of the button element
//
//	// Add to canvas
//	// canvas->AddMeshNode(selectedButtonIndex);
//}

// __Pending
//// Keyboard: Might provide arrow key navigation but no necessary
//void MeshCreationPopupMenu::OnKeyboardButton(int key, int scancode, int action, int mods)
//{
//	// Change highlight according to current selection
//	// Activate button functionality using Enter key
//	//if ()
//	//{
//	//	// Add to canvas
//	//	canvas->AddMeshNode(selectedButtonIndex);
//	//}
//}