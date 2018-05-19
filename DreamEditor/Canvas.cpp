#include "Canvas.h"
#include "PropertyPanel.h"
#include "GraphManager.h"
#include "Constants.h"
#include "MaterialTextures.h"
#include <thread>
#include "Locales.h"
using std::thread;

// Static Variables Definition: Necessary for linking
CameraNode* Canvas::camera = NULL;
GLuint Canvas::imageTexture = NULL;

// Static Variables
float Canvas::imageCoords[8] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
float Canvas::cameraMovementSpeed = 0.1f;
float Canvas::DefaultLabelDisplayDuration = 5.f;	// In seconds
Sun* Canvas::sun = NULL;

void Canvas::Initialize()
{
	// Load Interface Texture
	char pathBuffer[MAX_PATH];
	imageTexture = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::CanvasFrameImagePath, pathBuffer));

	// Create Interface Elements
	decorationFrame = new Panel(GraphManager::windowWidth, GraphManager::windowHeight, imageCoords, imageTexture, this);
	informationLabel = new TextLabel((unsigned short*)Canvas_Information_Welcome, true, this);
	labelDisplayTime = 0;

	terrain = new Terrain();

	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);

	// Initialize Sun
	sun = new Sun(this);
	Time currentTime;
	GraphManager::FillWithCurrentTime(&currentTime);
	currentSunTiming = ((double)currentTime.hour + currentTime.minute/60 + currentTime.second/3600)/24.0*360.0 - 6.0/24.0*360;	// Initialize sum timing to current system time;  -6.0/24.0*360 because our sun rises at 6 o'clock a.m.
	if (currentSunTiming < 0.f)
	{
		currentSunTiming += 360.0;
	}
}

Canvas::~Canvas()
{
	// Delete Camera
	delete camera;

	// Delete Image Nodes
	{
		ImageNode* inode = (ImageNode*)(imageNodes.First());
		while (inode != NULL)
		{
			delete inode;
			inode = (ImageNode*)(imageNodes.Next());
		}
	}

	// Delete Text Nodes
	{
		TextNode* tnode = (TextNode*)(textNodes.First());
		while (tnode != NULL)
		{
			delete tnode;
			tnode = (TextNode*)(textNodes.Next());
		}
	}

	// Delete Mesh Nodes
	{
		MeshNode* mnode = (MeshNode*)(meshNodes.First());
		while (mnode != NULL)
		{
			delete mnode;
			mnode = (MeshNode*)(meshNodes.Next());
		}
	}

	// Delete Inerface Elements
	delete decorationFrame;
	delete informationLabel;

	// Textre is managed by MaterialTexture object
}

void Canvas::Render()
{
	// Render to the Shadow Framebuffer
	{
		// Bind to Shadow Frambuffer
		sun->UseFBO();

		// Set Viewport
		glViewport(0, 0, ShadowMapWidth, ShadowMapHeight);

		// Set Draw State
		drawState = 4;

		// Render Text and Image Nodes
		RenderVisibleNodes();	// Seriously I don't remember why this can effectively avoid MeshNodes being rendered

		// Render Selected MeshNodes
		MeshNode* node = (MeshNode*)(meshNodes.First());
		while (node)
		{
			if (!node->bInvisible && !node->bSleep && node->GetProgram() == PhongShadingProgram)
			{
				node->Render();
			}
			node = (MeshNode*)(meshNodes.Next());
		}

		// Hacking: Set viewport back: this is a hcking because it shouldn't be here: Ideally Sun would do that but we need access to CanvasNodes
		glViewport(viewportX, viewportY, viewportW, viewportH);
	}

	// Render to the camera
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3D Objects
		drawState = 3;

		// Render Nodes
		terrain->Render();
		RenderVisibleNodes();

		// 2D interface
		drawState = 2;

		// Lastly Render Interface
		decorationFrame->Render();
		// dropDownButton->Render();
		if (labelDisplayTime < DefaultLabelDisplayDuration)
		{
			informationLabel->UpdateTransparancy(labelDisplayTime > (DefaultLabelDisplayDuration - 1) ? (DefaultLabelDisplayDuration - labelDisplayTime): 1.0f);	// Set transparancy Animation: LInearyly fade out in the last second
			informationLabel->Render();
			labelDisplayTime += GraphManager::deltaTime;	// Forward time steps
		}
		static GLuint debugTextureNo = 0;	// USage: Use Visual Studio BreakPoint to manually update this value to whatever you want to idsplay (e.g. Refer to Material.cpp which one it is using)
	}
}

//  Check the distance of all objects from the camera, and update(genrate renderabls) for Canvas Nodes and Load Mesh Data Appropriately
void Canvas::Update()
{
	// Update Sun
	currentSunTiming += 1. / 3600.;	// 6 hours per day -> 360 degrees / (6hrs * 3600s * 60frames/sec) -> 1/3600 degree/frame
	//currentSunTiming += 1. / 5.;	// Debug
	if (currentSunTiming > 360.) currentSunTiming = 0.;
	sun->UpdateSun(currentSunTiming);

	// Text Input Resizing Animation
	if (dynamic_cast<TextNode*>(currentSelection) || dynamic_cast<ImageNode*>(currentSelection))
	{
		currentSelection->Update();
	}

	// Smooth Camera Navigation _Landscape Style
	if((KeyStates[CanvasKeyControls::Forward]
		|| KeyStates[CanvasKeyControls::Backward]
		|| KeyStates[CanvasKeyControls::Left]
		|| KeyStates[CanvasKeyControls::Right])
		&& currentSelection == NULL)
	{
		float Ry = camera->GetRotationY();
		float accleration = cameraMovementSpeed * (KeyStates[CanvasKeyControls::Shift] ? 5 : 1);
		camera->Translate(-accleration * glm::sin(Ry) * KeyStates[CanvasKeyControls::Forward], 0.f, -accleration * glm::cos(Ry) * KeyStates[CanvasKeyControls::Forward]);	// W
		camera->Translate(-accleration * glm::cos(Ry) * KeyStates[CanvasKeyControls::Left],	0.f, accleration * glm::sin(Ry) * KeyStates[CanvasKeyControls::Left]);	// A
		camera->Translate(accleration * glm::sin(Ry) * KeyStates[CanvasKeyControls::Backward],0.f, accleration * glm::cos(Ry) * KeyStates[CanvasKeyControls::Backward]);	// S
		camera->Translate(accleration * glm::cos(Ry) * KeyStates[CanvasKeyControls::Right],	0.f, -accleration * glm::sin(Ry) * KeyStates[CanvasKeyControls::Right]);	// D
		// For not so obvious reason the above way, although a bit more expensive, works better than the below methods, although algebraically they look the same.

		camera->UpdateMatrix();
		UpdateNodeRenderState();
	}
}

void Canvas::Save(FILE* file)
{
	// Save Camera Location
	camera->SaveData(file);

	// Save Image Nodes
	SaveDispatch(&imageNodes, file);

	// Save Text Nodes
	SaveDispatch(&textNodes, file);

	// Save Mesh Nodes
	SaveDispatch(&meshNodes, file);
}

void Canvas::ExportAll()
{
	// Image Nodes
	{
		ImageNode* inode = (ImageNode*)(imageNodes.First());
		unsigned int order = 1;
		while (inode != NULL)
		{
			inode->Export();
			inode = (ImageNode*)(imageNodes.Next());
			order++;
		}
	}

	// Text Nodes
	{
		TextNode* tnode = (TextNode*)(textNodes.First());
		while (tnode != NULL)
		{
			tnode->Export();
			tnode = (TextNode*)(textNodes.Next());
		}
	}
}

// Object management
// Add ImageNode to the global management list so futher updates and rendering will see it, also for saving use
void Canvas::AddImageNode(ImageNode* imageNode)
{
	imageNodes.Add(imageNode);
}

void Canvas::AddTextNode(TextNode* textNode)
{
	textNodes.Add(textNode);
}

void Canvas::AddMeshNode(MeshNode* meshNode)
{
	meshNodes.Add(meshNode);
}

void Canvas::AddMeshNode(unsigned short meshIndex)
{

}

float* Canvas::GetSunColor()
{
	return sun->GetColor();
}

float* Canvas::GetSkyColor()
{
	return sun->GetSky();
}

float* Canvas::GetSunDirection()
{
	return sun->GetDirection();
}

glm::mat4& Canvas::GetSunProjection()
{
	return sun->GetProjection();
}

glm::mat4& Canvas::GetSunView()
{
	return sun->GetView();
}

GLuint Canvas::GetShadowMap()
{
	return sun->GetShadowMap();
}


// Object InteractionManagement
bool Canvas::MouseButtonDispatch(LinkedList* nodeList, int button, int action, int mods, double xpos, double ypos)
{
	CanvasNode* node = (CanvasNode*)(nodeList->First());
	while (node && !node->bSleep)
	{
		float nodeX, nodeY;
		if (CheckObjectCollision(node, xpos, ypos, &nodeX, &nodeY))
		{
			// Order matters due to the way we implement it: OnMouseButton function will summon and display the cursor, so call it after selection has been made
			ChangeCurrentSelection(node);
			node->OnMouseButton(button, action, mods, nodeX, nodeY);
			return true;
		}

		node = (CanvasNode*)(nodeList->Next());
	}

	ChangeCurrentSelection(NULL);
	return false;
}

bool Canvas::MouseOverDispatch(LinkedList* nodeList, double xpos, double ypos)
{
	CanvasNode* node = (CanvasNode*)(nodeList->First());
	while (node && !node->bSleep)
	{
		float nodeX, nodeY;
		if (CheckObjectCollision(node, xpos, ypos, &nodeX, &nodeY))
		{
			node->OnMouseOver(nodeX, nodeY);
			return true;;
		}
		else if (node->bMouseOverNotified)
		{
			node->OnMouseLeave();
			return true;
		}

		node = (CanvasNode*)(nodeList->Next());
	}

	return false;
}

void Canvas::RenderVisibleNodes()
{
	CanvasNode* node = visibleNodes.First();
	while (node != NULL)
	{			
		node->Render();
		node = visibleNodes.Next();
	}
}

void Canvas::SaveDispatch(LinkedList* nodeList, FILE* file)
{
	CanvasNode* node = (CanvasNode*)(nodeList->First());
	while (node != NULL)
	{
		node->SaveData(file);
		node = (CanvasNode*)(nodeList->Next());
	}
}

// Called whenever camera moves
void Canvas::UpdateNodeRenderState()
{
	visibleNodes.Clear();
	UpdateDispatch(&textNodes);
	UpdateDispatch(&meshNodes);
	UpdateDispatch(&imageNodes);
}

void Canvas::UpdateDispatch(LinkedList* nodeList)
{
	CanvasNode* node = (CanvasNode*)(nodeList->First());
	while (node != NULL)
	{
		if (node->bPendingDeletion)
		{
			// Continue Next
			node = (CanvasNode*)(nodeList->Next());
			continue;
		}

		// We could have devised a way to make use of auto-calculated polysoup but our current design doesn't have a polysoup available until the mesh is first loaded, so we require an input from the artist
		float distance = ToCameraDistance(node);

		// For MeshNodes
		if (MeshNode* mNode = dynamic_cast<MeshNode*>(node))
		{
			float toCameraDistance = glm::abs(distance) - mNode->GetSize();

			// If camera is inside Mesh Boundardy
			if (toCameraDistance < 0)
			{
				// If asleep, wake it up and mark visible
				if (mNode->bSleep)
				{
					mNode->WakeUp();
				}

				if (ToCameraPlaneDistance(mNode) >(-mNode->GetSize()))	// In front of camera
				{
					mNode->bInvisible = false;
					visibleNodes.Add(node, distance);
				}
			}
			// If camera is outside Mesh Boundary and far into fog
			else if (toCameraDistance > fogDepth)
			{
				if (!mNode->bSleep)
				{
					node->FallAsleep();
					node->bInvisible = true;
				}
			}
			// If camera is outside Mesh Boundary and distance is within viewable range
			else
			{
				if (node->bSleep)
				{
					node->WakeUp();
				}

				// Show the mesh if infront of camera
				if (ToCameraPlaneDistance(mNode) > (-mNode->GetSize()))	// In front of camera
				{
					node->bInvisible = false;
					visibleNodes.Add(node, distance);
				}
				else
				{
					node->bInvisible = true;
				}
			}
		}
		// For Text and Image nodes
		else
		{
			// If outside camera visible range and not asleep, put it to sleep
			if (abs(distance) > fogDepth && !node->bSleep)
			{
				node->FallAsleep();
				node->bInvisible = true;
			}

			// If inside camera visible range and asleep, wake it up
			if (abs(distance) < fogDepth)
			{
				if (node->bSleep)
				{
					node->WakeUp();
				}

				// Not working well: See Monkey Scene
				// If behind camera, set invisibility
				if (distance < 0)	// Hemispherical Position
				{
					node->bInvisible = true;
				}
				else
				{
					node->bInvisible = false;

					// Also will be rendered in this case
					visibleNodes.Add(node, distance);
				}

			}
		}
		
		// Continue Next
		node = (CanvasNode*)(nodeList->Next());
	}
}

// Interface Implementation
void Canvas::OnMouseOver(double xpos, double ypos)
{
	if (bMMDown)
	{
		// Common Rotation Factors
		float dHori = xpos - mouseX;
		float dVerti = ypos - mouseY;

		mouseX = xpos;
		mouseY = ypos;

		// Rotation Delta
		float aspectRatio = ((float)viewportW / viewportH);
		float scaleFactor = 120.0f * 3;	// Camera Rotation speed control
		float rotationY = -dHori / scaleFactor / aspectRatio * 3.14f;

		// If current selection and MeshNode
		// Do rotation: We need do rotating in Canvas space because we wish to use MouseOver to control angle; And we are using MMB to do rotating because we cannot easily use a keyboard button(e.g. R) to activate rotation mode since if there is any current selection, it will capture all Keyboard Events by design
		// so Canvas actually doesn't have a chance to process it because Canvas doesn't know whether or not this "R" press is processed by the CanvasNode(Well actually that isn't so true)
		if (currentSelection)
		{
			if (MeshNode* meshNode = dynamic_cast<MeshNode*>(currentSelection))
			{
				// Reqeust for implementing text container
				currentSelection->Rotate(0, rotationY, 0);	// Only rotate MeshNode since other nodes are not designed for that (Or basically it is that TextContainer hasn't implemented that function)
			}
		}
		else
		{
			// Camera Rotation Implementation
			//camera->Rotate(0, rotationY, 0);*/
			camera->Rotate(dVerti / scaleFactor / aspectRatio * 3.14f, rotationY, 0);	// 120.f controls amplitute
			camera->UpdateMatrix();

			UpdateNodeRenderState();

			// Update Mainmenu Panel Display
			GraphManager::mainMenu->LayerRotate(rotationY);
		}
	}
	else
	{

		if (currentSelection && KeyStates[CanvasKeyControls::MoveNode])
		{
			// Translation Delta calculation
			float dHori = xpos - lastXPos;
			float dVerti = ypos - lastYPos;
			float aspectRatio = ((float)viewportW / viewportH);
			float translationZ = -dVerti / 20;	// Z maps to vertical direction
			float translationX = dHori * aspectRatio / 20;	// X maps to horizontal direction

			// Otherwise translate canvs nodes
			currentSelection->Translate(translationX, 0, translationZ);
		}
		else
		{
			// CanvasNode mouse over events
			CanvasNode* node = visibleNodes.Last();
			while (node)
			{
				float nodeX, nodeY;
				if (CheckObjectCollision(node, xpos, ypos, &nodeX, &nodeY))
				{
					node->OnMouseOver(nodeX, nodeY);
					break;
				}
				else if (node->bMouseOverNotified)
				{
					node->OnMouseLeave();
					break;
				}

				node = visibleNodes.Prev();
			}
		}
	}

	// Save For OnDropWindow Use
	lastXPos = xpos;
	lastYPos = ypos;
}

void Canvas::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	// Rotate Camera Is Always needed
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		bMMDown = true;

		GraphManager::GrabCursor();

		mouseX = xpos;
		mouseY = ypos;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		bMMDown = false;

		GraphManager::ReleaseCursor();
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL))
	{
		// Text Node Creation
		Vector3f world;
		float seperation;
		if (CheckObjectCollision(terrain, xpos, ypos, NULL, NULL, &world, &seperation))
		{
			if (seperation <= fogDepth)
			{
				// Create a TextNode
				TextNode* newText = new TextNode(world.x - 0.8, world.y + CameraNode::CameraHeight, world.z);	// The value 0.8 is heuristic, just to make sure the quote symol appear above the cursor
				AddTextNode(newText);
				newText->WakeUp();
				ChangeCurrentSelection(newText);
				UpdateNodeRenderState();
				GraphManager::ShowTextNodeProperty(newText);
			}
		}
	}
	else
	{

		// Modern Method
		CanvasNode* node = visibleNodes.Last();
		while (node)
		{
			float nodeX, nodeY;
			if (CheckObjectCollision(node, xpos, ypos, &nodeX, &nodeY))
			{
				// Order matters due to the way we implement it: OnMouseButton function will summon and display the cursor, so call it after selection has been made
				ChangeCurrentSelection(node);
				node->OnMouseButton(button, action, mods, nodeX, nodeY);	// GraphManager::Show????NodeProperty() is called inside the node itself
				break;
			}

			node = visibleNodes.Prev();
		}

		// If we didn't find anything new node during collision
		if (currentSelection != node)	// -> Equivalent to if(node == NULL)
		{
			ChangeCurrentSelection(NULL);
			if (action == GLFW_PRESS) GraphManager::ShowDocumentProperty();	// Here we check GLFW_PRESS again because previously we didn't check it because entering this code area, now it seems not necessary

			UpdateInformationLabelText((unsigned short*)Canvas_Information_UsageTip_CreateTextNode);
		}
	}
}

void Canvas::OnKeyboardButton(int key, int scancode, int action, int mods)
{
	// Update Movement Key States
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			KeyStates[CanvasKeyControls::Forward] = true;
			if (!(mods & GLFW_MOD_SHIFT))
			{
				UpdateInformationLabelText((unsigned short*)Canvas_Information_UsageTip_ShiftToAccelerate);
				// DO this only when pressing forward since keys like 'S' might be used when save documents
			}
			break;
		case GLFW_KEY_A:
			//	if (!(mods & GLFW_MOD_SHIFT))// Deprecated for use with MeshCreationPop
				KeyStates[CanvasKeyControls::Left] = true;
			break;
		case GLFW_KEY_S:
			KeyStates[CanvasKeyControls::Backward] = true;
			break;
		case GLFW_KEY_D:
			KeyStates[CanvasKeyControls::Right] = true;
			break;
		case GLFW_KEY_G:
			KeyStates[CanvasKeyControls::MoveNode] = true;
			break;
		default:
			break;
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			KeyStates[CanvasKeyControls::Forward] = false;
			break;
		case GLFW_KEY_A:
			KeyStates[CanvasKeyControls::Left] = false;
			break;
		case GLFW_KEY_S:
			KeyStates[CanvasKeyControls::Backward] = false;
			break;
		case GLFW_KEY_D:
			KeyStates[CanvasKeyControls::Right] = false;
			break;
		case GLFW_KEY_G:
			KeyStates[CanvasKeyControls::MoveNode] = false;
			break;
		default:
			break;
		}
	}
	KeyStates[CanvasKeyControls::Shift] = (mods & GLFW_MOD_SHIFT);

	// Exit Current Selection etc.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		ChangeCurrentSelection(NULL);
		if (action == GLFW_PRESS) GraphManager::ShowDocumentProperty();
	}

	// Node Input
	if (currentSelection != NULL)
	{
		// Input into nodes
		currentSelection->OnKeyboardButton(key, scancode, action, mods);
		if (currentSelection->bPendingDeletion)
		{
			// Then do not render it. Actual code implementation is in CanvasNode's Render() function code
			// Also switch to Document Panel
			ChangeCurrentSelection(NULL);
			GraphManager::ShowDocumentProperty();
		}
	}
	// Auxiliary Movements_Debug Purpose: __Release 0.75__ Used because this makes editing easier somtimes
	else if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT ) && currentSelection == NULL)	// Don't receive GLFW_Press case to avoid delay
	{
		camera->Translate(0, -cameraMovementSpeed/2, 0);
		camera->UpdateMatrix();
		UpdateNodeRenderState();
	}
	else if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT) && currentSelection == NULL)
	{
		camera->Translate(0, cameraMovementSpeed/2, 0);
		camera->UpdateMatrix();
		UpdateNodeRenderState();
	}
	else if (key == GLFW_KEY_R && action == GLFW_PRESS && currentSelection == NULL)
	{
		camera->Rotation(0, 0, 0);
		camera->UpdateMatrix();
		UpdateNodeRenderState();

		// Debug Label Functioning
		UpdateInformationLabelText((unsigned short*)L"Rest Camera Rotation!");
	}
}

void Canvas::OnTextInput(unsigned int codepoint)
{
	// Only Useful for TextNode or Image Node
	if (dynamic_cast<TextNode*>(currentSelection) != NULL || dynamic_cast<ImageNode*>(currentSelection) != NULL)
	{
		currentSelection->OnTextInput((unsigned short*)&codepoint,1);
	}
}

void Canvas::OnDropWindow(int count, const char** paths)
{
	// __Pending: When nodes are dropped, it should face camera orientation by default

	// Update Data
	for (int i = 0; i < count; i++)
	{
		// Convert to U16
		std::wstring utf16 = SystemFunctions::ConvertU8toU16(paths[i]);

		// Convert to ANSI
		int size = SystemFunctions::U16ToANSI((unsigned short*)utf16.c_str(), utf16.length(), NULL, NULL);
		char* ANSIPath = (char*)malloc(size + 2);	// Assume ANSI use two bytes for NULL
		SystemFunctions::U16ToANSI((unsigned short*)utf16.c_str(), utf16.length(), ANSIPath, size);
		ANSIPath[size] = 0;
		ANSIPath[size + 1] = 0;

		// Image?
		if (GraphManager::IsImageFile(ANSIPath))
		{
			cout << "[Debug] Detected ImageFile Drop in" << endl;

			// Image Node Creation
			Vector3f world;
			float seperation;
			if (CheckObjectCollision(terrain, lastXPos, lastYPos, NULL, NULL, &world, &seperation))
			{
				if (seperation <= fogDepth)
				{
					// Create a ImageNode
					ImageNode* newImage = new ImageNode(world.x - 0.8, world.y + CameraNode::CameraHeight*1.25/*Make image nodes a bit higher*/, world.z, ANSIPath);	// The value 0.8 is heuristic, just to make sure the quote symol appear above the cursor
					AddImageNode(newImage);
					newImage->WakeUp();
					ChangeCurrentSelection(newImage);
				}
			}
		}
		// Mesh?
		else if (GraphManager::IsMeshSpecFile(ANSIPath))
		{
			cout << "[Debug] Detected MeshSPecFile Drop in" << endl;
			CreateMeshNode(ANSIPath);
		}
		// Map?
		else if (GraphManager::IsDreamMapFile(ANSIPath))
		{
			cout << "[Debug] Detected MapFIle Drop in" << endl;
		}
		else
		{
			// Nothing happened
		}

		UpdateNodeRenderState();
	}
}

void Canvas::OnResizeFramebuffer(int width, int height)
{
	// Interface Properties
	viewportX = 0;	// We no longer consider PropertyPanel width because now we use floating panels
	viewportY = 0;
	viewportW = width;
	viewportH = height;

	// Manually Layout Things
	decorationFrame->UpdatePosition(0, 0);
	informationLabel->UpdatePosition(viewportW - informationLabel->GetDimensionX() - 20, -(int)viewportH + 40);

	// Update 2D Projection and View
	UpdateProjection();
	// Update Camera Projection and View
	if (camera)
	{
		camera->UpdateMatrix();
	}
}

glm::mat4x4& Canvas::GetViewMatrix()
{
	switch (drawState)
	{
	case 2:
		return view2D;
		break;
	case 3:
		return camera->view;
		break;
	case 4:
		return sun->GetView();
		break;
	}
}

glm::mat4x4& Canvas::GetProjectMatrix()
{
	switch (drawState)
	{
	case 2:
		return project2D;
		break;
	case 3:
		return camera->project;
		break;
	case 4:
		return sun->GetProjection();
		break;
	}
}

// Interface Interaction Functions
void Canvas::SetCamera(CameraNode* inCamera)
{
	camera = inCamera;
}

CameraNode* Canvas::GetCamera()
{
	return camera;
}

void Canvas::CreateMeshNode(char* ANSIPath)
{
	// Final Filename
	const char* filename = NULL;	// Will store pure form of file location

	// Processing filepath
	// Do necessary path trncating and file manipulation, to make it in "pure form"
	// Check Whether inside Data Directory
	bool bRelativePath = false;	// Normally this shouldn't be the case considering how this constructor will get called
	char pathBuffer[MAX_PATH];
	if (!SystemFunctions::IsFileInDataDiretory(ANSIPath, pathBuffer, &bRelativePath, &filename) && !bRelativePath)	// Depend on side effect
	{
		// Make a Copy or all relevent files, including the specFile itself, and the shape and textures and collision file Then extract pure form fileanme
		// But that was to complicated, so we simply warn the user of the wrong location of the file and expect him to be smart enough to copy all relavant files himeself

		// __Pending: Make use of debug text label this time

		return;	// Return without creating anything
	}
	else if (bRelativePath)
	{
		// If it is a relative path, Do some extra cleaning: remove ". \"
		// Or, report an error since it shouldn't be
		cout << "[Error]How come input file is a relative path?!" << endl;
		return;	// Return without creating anything
	}

	// There are two cases of abs path: one is inside data dir, the other is not. The latter has been handle already, now it will be the case when it is a file inside data dir
	// Continue creation

	// Mesh Node Creation
	Vector3f world;
	float seperation;
	if (CheckObjectCollision(terrain, lastXPos, lastYPos, NULL, NULL, &world, &seperation))
	{
		if (seperation <= fogDepth)
		{
			MeshNode* newMesh = new MeshNode(world.x, world.y/* + CameraNode::CameraHeight: Do not do this manually on the mesh when needed, the content creator shall do this so he has full contorl*/, world.z, filename);
			AddMeshNode(newMesh);
			newMesh->WakeUp();
			// currentSelection = newMesh;
		}
	}
}

void Canvas::ChangeCurrentSelection(CanvasNode* newSelection = NULL)
{
	if (currentSelection)
	{
		currentSelection->OnDeselection();
	}
	currentSelection = newSelection;
	//currentSelection->OnSelection();	// Not used since not needed
}

void Canvas::UpdateInformationLabelText(unsigned short* newText)
{
	informationLabel->UpdateContent(newText);
	informationLabel->UpdatePosition(viewportW - informationLabel->GetDimensionX() - 20, -(int)viewportH + 40);
	labelDisplayTime = 0.f;	// Reset clock
}

bool Canvas::CheckObjectCollision(CanvasNode* node, float cursorX, float cursorY, float* xpos, float *ypos, Vector3f* World, float* distance)
{
	// Get World Space Camera Ray
	float ratioX = (cursorX / viewportW - 0.5) * 2;
	float ratioY = ((viewportH + cursorY) / viewportH - 0.5) * 2;
	glm::vec4 rayClip = glm::vec4(ratioX, ratioY, -1 /*Here -1 can be anything*/, 1 /*This is our embeded z, which will be the z component for next step; but also can be anything since we will manually set it*/);
		// Notice this space is not exactly Clipping Space, but actually NDC Space: perspective division has been beglected/assumed. Here rayClip.x and rayClip.y corresponds to exactly the point with z = -1 in eye space. 
		// The general form of Xclip will be Xclip = Xndc * W = Xndc * Ze. Here we are using/assuming a Ze of -1 in Eye space, thus after projection w will be 1, and n is 1, so after Perspective Division the value we are using(ratioX and ratioY) will just correspond to point rayEye;
		// Let's say if we are using a rayEye of distance other than 1, then we need to multiply that value before assigning it to rayClip.
		// Notice near and far plane doesn't matter in this case.
	glm::vec4 rayEye = glm::inverse(camera->project) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.f, 0.f);	// Manually inverse Z

	glm::vec3 cameraRay = glm::normalize(glm::vec3(rayEye));	// Normalize
	glm::vec3 rayWor = glm::vec3(inverse(camera->view) * rayEye);
	// don't forget to normalise the vector at some point
	rayWor = glm::normalize(rayWor);

	if (dynamic_cast<TextNode*>(node) || dynamic_cast<ImageNode*>(node))
	{
		// Do Collision Detection in View Space

		// Instantiate Collision For Planar Nodes
		glm::vec3 P1 = glm::vec3(0, 0, 0);	// Remember the origin of a frame is 0,0,0 in obejct space
		glm::vec3 P2 = P1 + glm::vec3(node->width, 0, 0);
		glm::vec3 P3 = P1 + glm::vec3(node->width, -node->height, 0);
		glm::mat4x4 model = glm::translate(glm::mat4(1), node->GetWorldPosition());
		P1 = glm::vec3(camera->view * model * glm::vec4(P1, 1));
		P2 = glm::vec3(camera->view * model * glm::vec4(P2, 1));
		P3 = glm::vec3(camera->view * model * glm::vec4(P3, 1));
		glm::vec3 V1 = glm::normalize(P2 - P1);	// Gonna to be normalized, otherwise later when project to those two directions there will be problem
		glm::vec3 V2 = glm::normalize(P3 - P2);
		glm::vec3 N = glm::normalize(glm::cross(V2, V1));
		float D = -glm::dot(N, P1);
		float t = -D / (glm::dot(N, cameraRay));	// glm::dot(N, cameraRay) could be zero but we cannot test it since it's a float

		// Extract Useful Data
		glm::vec3 P = t*cameraRay;
		// float normalSeperation = glm::dot(N, P - P1); // Seemly non-sense calculation
		float t1 = glm::dot(V1, P - P1);
		float t2 = glm::dot(V2, P - P1);

		// Collision Detection
		if (t >= 0 && t1 >= 0 && t1 <= node->width && t2 >= 0 && t2 <= node->height)
		{
			if (xpos)
			{
				*xpos = t1;
			}
			if (ypos)
			{
				*ypos = -t2;
			}
			if (World)
			{
				World->x = P.x;
				World->y = P.y;
				World->z = P.z;
			}
			if (distance)
			{
				*distance = glm::length(camera->GetWorldPosition() - P);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	else if (MeshNode* meshNode = dynamic_cast<MeshNode*>(node))
	{
		// Do Collision Detection
		return meshNode->RayCollision(rayWor, camera->GetWorldPosition());
	}
	else if (dynamic_cast<Terrain*>(node))
	{
		// Do Collision Detection in View Space

		// Instantiate Collision For Terrrain
		glm::vec3 P1 = glm::vec3(0, 0, 0);
		glm::vec3 P2 = glm::vec3(1, 0, 0);
		glm::vec3 P3 = glm::vec3(0, 0, 1);
		// Assume No WorldTransform to our Terrain
		P1 = glm::vec3(camera->view * glm::vec4(P1, 1));
		P2 = glm::vec3(camera->view * glm::vec4(P2, 1));
		P3 = glm::vec3(camera->view * glm::vec4(P3, 1));
		glm::vec3 V1 = glm::normalize(P2 - P1);
		glm::vec3 V2 = glm::normalize(P3 - P2);
		glm::vec3 N = glm::normalize(glm::cross(V2, V1));

		// Collision Detection
		glm::vec3 cameraWorld = camera->GetWorldPosition();
		float D = -glm::dot(N, P2);
		float temp = glm::dot(N, cameraRay);
		float t = -D / (glm::dot(N, cameraRay));
		if (t < 0)
		{
			return false;
		}
		else
		{
			// Extract Useful Data
			glm::vec3 P = t*cameraRay;
			P = glm::vec3(inverse(camera->view) * glm::vec4(P,1));

			// Set Return Value
			if (xpos)
			{
				*xpos = 0;
			}
			if (ypos)
			{
				*ypos = 0;
			}
			if (World)
			{
				World->x = P.x;
				World->y = P.y;
				World->z = P.z;
			}
			if (distance)
			{
				*distance = glm::length(cameraWorld - P);
			}
			return true;
		}
	}
	else
	{
		return false;
	}
}

float Canvas::ToCameraDistance(CanvasNode* node)
{
	// Calculate Node Facing in Eye Space
	//glm::vec3 rayEye = glm::vec3(camera->view * glm::vec4(0.0f, 0.0f, -1.0f, 0.f));	// __Debug__: Notice due to our configuration glm::vec4(0.0f, 0.0f, -1.0f, 0.f) isn't the view space vector?!
	glm::vec3 rayEye = glm::vec3(0.0f, 0.0f, -1.0f);	// __Debug__: Notice due to our configuration glm::vec4(0.0f, 0.0f, -1.0f, 0.f) isn't the view space vector?!
	glm::vec3 nodeEye = glm::vec3(camera->view * glm::vec4(node->GetWorldPosition(), 1));	// Notice we didn't normalize nodeEye since we will need its length, and also the dot product below doesn't need actual value so it is OK this vector isn't normalized

	int faceCamera = glm::dot(rayEye, nodeEye) > 0.f ? 1 : -1;
	return glm::length(nodeEye - rayEye)*faceCamera;
}

float Canvas::ToCameraPlaneDistance(CanvasNode* node)
{
	// Reference: http://mathinsight.org/distance_point_plane, if normal vector is normalized, then no need to do the division
	glm::vec3 rayEye = glm::vec3(camera->view * glm::vec4(0.0f, 0.0f, -1.0f, 0.f));	// This is the normal of the camera plane
	float D = glm::dot(-rayEye, glm::vec3(camera->view * glm::vec4(1.f, 0.f, 0.f, 0.f)));
	glm::vec3 nodeEye = glm::vec3(camera->view * glm::vec4(node->GetWorldPosition(), 0));	// Do not normalize

	return glm::dot(rayEye, nodeEye) + D;
}