#pragma once
#include "Interface.h"
#include "CanvasNodes.h"
#include "LinkedList.h"
#include "DropDownButton.h"
#include "Panel.h"
#include "TextLabel.h"
#include "TerrainNode.h"
#include "World.h"
#include "Sun.h"
#include "CanvasNodeSortedLinkedList.h"
// Debug
#include "RenderableDebugRay.h"

enum CanvasKeyControls
{
	Forward = 0,
	Backward,
	Left,
	Right,
	Shift,
	MoveNode,	// Canvas node movement
	NumControls
};

class Canvas : public Interface, public World	// Double Inheritance!
{
public:
	// Initialize Canvas Elements, and Initialize Canvas ENvironment
	void Initialize();
	~Canvas();	// Delete Canvas Nodes and Canvas Interface Elements

	// Render Request
	virtual void Render();
	/* Render All 3D nodes first, then Render the interface; Camera Position thus ViewMatrix is already constructed when loading and updating nodes*/

	/* During Update Lots of things happen:
		- Potentially suported animations; For shader times, every renderable has access to a global timer so no need for us to do that
		- For mesh/text/image nodes, even though all the important specification DATA are loaded at initialization time, openGL objects need to be created
		and destoryed at runtime for maximum effciency
	*/
	// Update: go through all the CanvasNodes and Decide Streaming*/
	void Update();
	// Save: Save All Canvas Nodes informatoin into a file
	void Save(FILE* file); 
	void ExportAll(); // Export Raw images and Texts only, in a folder named after the document

	// Canvas Space Interactions
	// Mouse: Node Editing
	void OnMouseOver(double xpos, double ypos);
	// Mouse: Node Detail Editing and Node Selection
	void OnMouseButton(int button, int action, int mods, double xpos, double ypos);
	// Keyboard: Shortcut and Node Management
	void OnKeyboardButton(int key, int scancode, int action, int mods);
	// Keyboard: Text String Input
	void OnTextInput(unsigned int codepoint);
	// File System Interaction: Drop in Images and Mesh Specs
	void OnDropWindow(int count, const char** paths);
	// Framebuffer resizing
	virtual void OnResizeFramebuffer(int width, int height);
	virtual glm::mat4x4& GetViewMatrix();
	virtual glm::mat4x4& GetProjectMatrix();

	// Specific Interface Functionalities
	void SetCamera(CameraNode* inCamera);
	static CameraNode* GetCamera();

	// Objects Management
	void AddImageNode(ImageNode* imageNode);
	void AddTextNode(TextNode* textNode);
	void AddMeshNode(MeshNode* meshNode);	// By design, at the time of this call, meshResources are not fully loaded
	void AddMeshNode(unsigned short meshIndex);	// Used specifically with MeshCreationPopupMenu: Use camera position to position the node; At the time of this function, meshResources have been fully loaded
	void UpdateRenderState(){ UpdateNodeRenderState(); }

	// Object Search Facility
	// __release 0.75__ :Since we are not so familiary with using threads, in this case we use simple search instead
	//void SearchText(unsigned short* search);
	//void GotoNextFoundNode();

	// Rendering Facility
	static float* GetSunColor();
	static float* GetSkyColor();
	static float* GetSunDirection();	// Return in world space
	static glm::mat4& GetSunProjection();
	static glm::mat4& GetSunView();
	static GLuint GetShadowMap();

	// Facilitate  interface
	void UpdateInformationLabelText(unsigned short* newText);

private:
	// Object Ineraction Management
	bool MouseButtonDispatch(LinkedList* nodeList, int button, int action, int mods, double xpos, double ypos);
	bool MouseOverDispatch(LinkedList* nodeList, double xpos, double ypos);
	void RenderVisibleNodes();
	void SaveDispatch(LinkedList* nodeList, FILE* file);
	void UpdateNodeRenderState();
	void UpdateDispatch(LinkedList* nodeList);
	void CreateMeshNode(char* ANSIPath);	// Used by OnDropWindow()
	void ChangeCurrentSelection(CanvasNode* newSelection);

private:
	// Canvas Logics
	// bool ShouldCull(CanvasNode* node);	// Check Camera Culling
	bool CheckObjectCollision(CanvasNode* node, float cursorX, float cursorY, float* xpos = NULL, float *ypos = NULL, Vector3f* World = NULL, float* distance = NULL);	// Return collide or not, and CanvasObject space x,p pos; distance is the distance to the point of collision from camera
	float ToCameraDistance(CanvasNode* node);	// Return a negative if behind(If the anble between the distance vector and view vector is bigger than 90)
	float ToCameraPlaneDistance(CanvasNode* node);
	double mouseX, mouseY;	// For MMB behavior usage only
	bool bMMDown = false;
	int drawState;	// 2: 2D; 3:3D; 4: Shadow
	double currentSunTiming = 0;	// In 360 degerees time frame
	bool KeyStates[CanvasKeyControls::NumControls];

private:
	// Bookeeping
	CanvasNode* currentSelection = NULL;
	double lastXPos, lastYPos;
	float labelDisplayTime = 0;	// In seconds

	// Objects
	static CameraNode* camera;
	static Sun* sun;
	Terrain* terrain;
	LinkedList imageNodes;
	LinkedList textNodes;
	LinkedList meshNodes;
	CanvasNodeSortedLinkedList visibleNodes;

	// __Debug Use
	//RenderableDebugRay* worldOrigin;	// To Remind Ourselves it is not tempTextNode but Camera Spawn point! in current debug case
	//RenderableImageRect* debugImage;

private:
	// Interface Elements - Potential Candidates: a frame panel as decoration, a frame panel as vineette, a lower-right hand side bebug text label
	Panel* decorationFrame;
	// __Pending DropDownButton* dropDownButton;
	TextLabel* informationLabel;	// Will be used for informative purposes in final release

	static float imageCoords[8];
	static float cameraMovementSpeed;
	static float DefaultLabelDisplayDuration;
	static GLuint imageTexture;
};