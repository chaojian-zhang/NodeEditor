#include "GraphManager.h"
#include <time.h>

// Static Variables Definition: Necessary for linking, Others are required to initialize for application purpose
char* GraphManager::filename;
Time GraphManager::createTime;
Time GraphManager::modifyTime;
ShaderResource* GraphManager::shaderResource= NULL;
ImageResource* GraphManager::imageResource = NULL;
MeshResource* GraphManager::meshResource = NULL;
FontManager* GraphManager::font = NULL;
MainMenu* GraphManager::mainMenu = NULL;
Canvas* GraphManager::canvas = NULL;
double GraphManager::systemTime = 0.;
double GraphManager::deltaTime = 0.;
long GraphManager::frame = 0;
int GraphManager::windowWidth = 1024;
int GraphManager::windowHeight = 768;
int GraphManager::desktopWidth = 1024;
int GraphManager::desktopHeight = 768;
float GraphManager::aspectRatio = 1;
GLuint GraphManager::currentProgram = 0;
GLFWcursor* GraphManager::windowCursors[NumCursors] = {};
InterfaceElement* GraphManager::activeInterfaceElement = NULL;
Interface* GraphManager::currentPopup = NULL;
PropertyPanel* GraphManager::currentPropertyPanel = NULL;
PropertyPanel* GraphManager::currentActivePanel = NULL;
float GraphManager::animationScale = 1.0f;
DocumentPropertyPanel* GraphManager::docPanel = NULL;
ImageNodePropertyPanel* GraphManager::imgPanel = NULL;
TextNodePropertyPanel* GraphManager::textPanel = NULL;
MeshNodePropertyPanel* GraphManager::meshPanel = NULL;
FileMenuPopupMenu* GraphManager::filePop = NULL;
WindowMenuPopupMenu* GraphManager::windowPop = NULL;
HelpMenuPopupMenu* GraphManager::helpPop = NULL;
MeshCreationPopupMenu* GraphManager::meshPop = NULL;
ShortcutsPopupScreen* GraphManager::shortcutsScreen = NULL;
AboutPopupScreen* GraphManager::aboutScreen = NULL;
bool GraphManager::bFullscreen = false;
GLFWwindow* GraphManager::window = NULL;
LinkedList GraphManager::mainMenuElements;
LinkedList GraphManager::propertyPanelElements;
LinkedList GraphManager::popupMenuElements;
LinkedList GraphManager::canvasElements;
bool GraphManager::bCursorLockedForCamera = false;

// Treat this as a library function
void CheckDevILError()
{
	ILenum error;
	while ((error = ilGetError()) != IL_NO_ERROR)
	{
		cout << "[Error] DevIL error " << error << ":" << iluErrorString(error) << endl;
		__debugbreak();
	}
}

GraphManager::GraphManager(char* fn, char* im)
{
	// Validate and Extract command line arguments: filename and interface mode option
	if (CheckFilename(fn))
	{
		filename = fn;
		bInitializeSuccess = true;
	}
	else
	{
		cout << "[Error] Filename is not permited by the application." << endl;
		bInitializeSuccess = false;
		return;
	}
	if (im != "default")
	{
		bFullscreen = GetInterfaceMode(im);
	}
	bInitializeSuccess = false;	// Default False

	// Open the file to read in Document Properties
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		cout << "[Error] Dream Map File cannot be opened. Check filename or path is correct." << endl;
		bInitializeSuccess = false;
		return;
	}
	unsigned char tempBuffer[4];
	size_t readResult = fread(tempBuffer, 1, 3, file);
	if (readResult == 3)
	{
		// Validate format
		if (memcmp(tempBuffer, FORMAT_IDENTIFIER, 3) != 0)
		{
			cout << "Although filename seems correct, the file format is not specified DreamMap file. Failed to load." << endl;
			bInitializeSuccess = false;
			return;
		}
		
		// Since the file is not a blank file, continue read file format version
		fread(tempBuffer, 1, 1, file);

		// Validate format version
		if (tempBuffer[0] != FORMAT_VERSION) // I discovered [-] as a nice symbol
		{
			cout << "File format version doesn't match current version of the application. \
					DreamMap file version is: " << tempBuffer[0] << 
					", Program version is: " << FORMAT_VERSION << endl;
			bInitializeSuccess = false;
			return;
		}

		// Read in basic information about the Graph Document
		// By design End of File should not appear and other file reading errors are not expected, so we will bpass that check to make codes cleaner
		// Read in General Block Type tag identifier
		fread(tempBuffer, 1, 1, file);
		if (tempBuffer[0] != GENERAL_BLOCK_TAG)
		{
			cout << "The DreamMap file is correputed: Internal Data Not Arrange in order." << endl;
			bInitializeSuccess = false;
			return;
		}
		// Read values to initialize GraphDocument properties
		fread(&createTime, sizeof(Time), 1, file);
		fread(&modifyTime, sizeof(Time), 1, file);
		// fread(&openTime, sizeof(Time), 1, file);
	}
	// Avoid check ferror or feof
	else
	{
		// The file is a blank file
		// So manually set up default dcument
		FillWithCurrentTime(&createTime);
		FillWithCurrentTime(&modifyTime);
		// FillWithCurrentTime(&openTime);
	}

	// Do Application Initialization First, Then Read Resources and Nodes, since we will need pointer to Canvas
	if (!InitializeApplication())
	{
		cout << "Failed to Initialize Application." << endl;
		bInitializeSuccess = false;
		fclose(file);
		return;
	}

	// Prepare to read DreamMap resources and Nodes
	// Allocate Resource Managers: Mesh, Image
	imageResource = new ImageResource();
	meshResource = new MeshResource();
	// If file also contain resource information, then continue read
	readResult = fread(tempBuffer, 1, 1, file);
	// readResult != EOF is wrong
	while (readResult != 0)	// feof or ferror has occured
	{
		ImageData* image;
		MeshSpec* mesh;
		CameraNode* camera;
		ImageNode* imageNode;
		TextNode* textNode;
		MeshNode* meshNode;

		switch (tempBuffer[0])
		{
		case IMAGE_DATA_BLOCK_TAG:	// Image Data
			// Create Obejct
			image = new ImageData(file);
			// Add to Resource
			imageResource->AddImage(image);
			break;
		case MESH_DATA_BLOCK_TAG:	// Mesh Data
			// Create Obejct
			mesh = new MeshSpec(file);
			// Add to Resource
			meshResource->AddMesh(mesh);
			break;
		case CAMERA_NODE_BLOCK_TAG:	// Camera Node
			// Create Object
			camera = new CameraNode(file);
			// Associate With Link
			canvas->SetCamera(camera);
			break;
		case IMAGE_NODE_BLOCK_TAG:	// Image Node
			// Create Object
			imageNode = new ImageNode(file);
			// Add to Canvas
			canvas->AddImageNode(imageNode);
			break;
		case TEXT_NODE_BLOCK_TAG:	// Text Node
			// Create Object
			textNode = new TextNode(file);
			// Add to Canvas
			canvas->AddTextNode(textNode);
			break;
		case MESH_NODE_BLOCK_TAG:	// Mesh Node
			// Create Object
			meshNode = new MeshNode(file);
			// Add to Canvas
			canvas->AddMeshNode(meshNode);
			break;
		default:	// Unidentified Data will interrupt reading flow, so throw an error
			cout << "[Error] Unexpcted tag identifier encountered at: " << ftell(file) - 1
				<< ", the file might be correupted." << endl;
			bInitializeSuccess = false;
			return;
		}

		// continue read, expect a tag identifier
		readResult = fread(tempBuffer, 1, 1, file);
	}
	// Close the file
	fclose(file);

	// Special Case: If we are loading a blank Map file then we need to manually set up a camera for Canvas use
	if (canvas->GetCamera() == NULL)
	{
		CameraNode* camera = new CameraNode();
		canvas->SetCamera(camera);
	}

	// Finalize MeshReources loading and Initialize Popup Menus
	PostInitializeInterface();
	canvas->UpdateRenderState();

	bInitializeSuccess = true;	// Finally we are done with initialization, not sure how long this will take?

	// Other Default Value Settings
	aspectRatio = (float)windowWidth / windowHeight;
	currentPropertyPanel = currentActivePanel = docPanel;
	currentPopup = NULL;

	// Hide Console for Release
#ifndef APPLICATION_DEBUG
	FreeConsole();
#endif
}

GraphManager::~GraphManager()
{
	// Destroy Interfaces
	// Interfaces
	delete mainMenu;
	delete canvas;
	// Panels
	delete docPanel;
	delete imgPanel;
	delete textPanel;
	delete meshPanel;
	// Popups
	delete filePop;
	delete windowPop;
	delete helpPop;
	delete meshPop;
	// Displays
	delete shortcutsScreen;
	delete aboutScreen;

	// The order does matter: Since the above objects might refer to the data(image and mesh data) that are managed by the below objects
	// Release Resources: ShaderResource, MeshResource, ImageResource, Font
	delete shaderResource;
	delete imageResource;
	delete meshResource;
	MaterialTextures::DeleteTextureResources();
	delete font;

	// Close Libraries
	CloseLibraries();
}

void GraphManager::Begin()
{
	// Program Main Loop: Process Events and Handle Dynamic Streaming and Finally do the rendering
	while (!glfwWindowShouldClose(window))
	{
		// <State Update>
		double lastSystemTime = systemTime;
		systemTime = glfwGetTime();
		deltaTime = systemTime - lastSystemTime;
		frame++;
		// <Dunamic Streaming>
		canvas->Update();
		// <Rendering>
		Render();
		// <Process Events>
		glfwPollEvents();
	}
}

// Function callbacks
// Window Callbacks
void GraphManager::OnWindowError(int error, const char* description)
{
	cout << "[Window Error]" << description << endl;
}

void GraphManager::OnMouseOver(GLFWwindow* window, double xpos, double ypos)
{
	// Check Fallin of interface then dispatch Mouse Over Event of the interface elements
	if (!bCursorLockedForCamera)
	{
		if (/*CheckFallIn(mainMenu, xpos, ypos)*/!(mainMenu->bHidden))	// Always check main menu if it isn't hiden, to avoid edge effect
		{
			NotifyMouseOverMainMenu(xpos, ypos);
		}

		if (currentPopup) // Stop highlight buttons whenever our mouse cursor move away from popup menu to any other interface
		{
			// Handle Mouse Over and Button Highlighting of Popup Menu interface elements
			NotifyMouseOver(&popupMenuElements, currentPopup, xpos, ypos);

			// Handle PopupMenu disappearing when move into other interfaces
			if (!CheckFallIn(currentPopup, xpos, ypos) && !CheckFallIn(mainMenu, xpos, ypos))
			{
				// Speically handle Popup Screens
				if (currentPopup != shortcutsScreen && currentPopup != aboutScreen)
				{
					currentPopup = NULL;
				}
			}
		}
		else if (currentPropertyPanel && CheckFallIn(currentPropertyPanel, xpos, ypos))
		{
			NotifyMouseOver(&propertyPanelElements, currentPropertyPanel, xpos, ypos);
		}
		else if (CheckFallIn(canvas, xpos, ypos))
		{
			// Convert to Interface openGL space
			xpos = xpos - canvas->viewportX;
			ypos = (windowHeight - (canvas->viewportH + canvas->viewportY)) - ypos;
			canvas->OnMouseOver(xpos, ypos);
		}
	}
	else
	{
		// Convert to Interface openGL space
		xpos = xpos - canvas->viewportX;
		ypos = (windowHeight - (canvas->viewportH + canvas->viewportY)) - ypos;
		canvas->OnMouseOver(xpos, ypos);
	}
}

void GraphManager::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (currentPopup && CheckFallIn(currentPopup, xpos, ypos))
	{
		NotifyMouseButton(&popupMenuElements, currentPopup, button, action, mods, xpos, ypos);
	}
	else if (currentPropertyPanel && CheckFallIn(currentPropertyPanel, xpos, ypos))
	{
		NotifyMouseButton(&propertyPanelElements, currentPropertyPanel, button, action, mods, xpos, ypos);
	}
	else if (CheckFallIn(mainMenu, xpos, ypos))
	{
		NotifyMouseButton(&mainMenuElements, mainMenu, button, action, mods, xpos, ypos);
	}
	else if (CheckFallIn(canvas, xpos, ypos))
	{
		// Specially Handle Canvas

		// Handle PopupScreen disappearing when click inside Canvas(this is needed when user open a tutorial/about screen, 
		//	since at this moment the mouse cursor will not be inside the screen)
		if (action == GLFW_PRESS && (currentPopup == shortcutsScreen || currentPopup == aboutScreen))
		{
			currentPopup = NULL;
		}
		else
		{
			// Notify Mouse Button if any
			if (!NotifyMouseButton(&canvasElements, canvas, button, action, mods, xpos, ypos))
			{
				// Convert to Interface openGL space
				xpos = xpos - canvas->viewportX;
				ypos = (windowHeight - ((int)canvas->viewportH + (int)canvas->viewportY)) - ypos;
				// Then(Otherwise) Send to canvas for further processing
				canvas->OnMouseButton(button, action, mods, xpos, ypos);
			}
		}
	}

	if (bCursorLockedForCamera)
	{
		canvas->OnMouseButton(button, action, mods, xpos, ypos);
	}
}

void GraphManager::OnKeyboardButton(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Document Functions
	if (key == GLFW_KEY_S && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL))
	{
		Save();
		canvas->UpdateInformationLabelText((unsigned short*)Canvas_Information_Save);
	}

	//else if (key == GLFW_KEY_F11 && action == GLFW_PRESS && (mods & GLFW_MOD_ALT))
	//{
	//	// Comment: In current version of GLFW there is no easy way to toggle fullscreen; Unless create initially as fullscreen
	//	// And we cannot reliably destroy current window since OpenGL context might be lost
	//	// Might be supported in GLFW 3.2

	//	// Toggle Imersive Style Full Screen
	//	bFullscreen = !bFullscreen;

	//	if (bFullscreen) {
	//		// Set window size for "fullscreen windowed" mode to the desktop resolution.
	//		glfwSetWindowSize(window, desktopWidth, desktopHeight-1);
	//		// Move window to the upper left corner.
	//		glfwSetWindowPos(window, 0, 0);
	//	}
	//	else {
	//		// Use start-up values for "windowed" mode.
	//		glfwSetWindowSize(window, 1024, 768);
	//		glfwSetWindowPos(window, 512, 384);
	//	}

	//	// Credit:http://gamedev.stackexchange.com/questions/38620/toggle-fullscreen-at-runtime, although our version is quite different
	//	// Or a more complex way: http://www.java-gaming.org/index.php?topic=34882.0
	//	/*
	//		long newWindow = glfwCreateWindow(w, h, title, glfwGetPrimaryMonitor(), window);
	//		glfwDestroyWindow(window);
	//		window = newWindow;
	//		// setup new window (callbacks, make context current and init GL state, etc)
	//	*/

	//}

	else if (key == GLFW_KEY_F10 && action == GLFW_PRESS && (mods & GLFW_MOD_ALT))
	{
		// Toggle Hide Menus
		SwitchFullScreen();
	}
	
	//  Interface Interactions
	else if (activeInterfaceElement)
	{
		// Send Keyboard Event to current active Interface Element
		activeInterfaceElement->OnKeyboardButton(key, scancode, action, mods);
	}
	else
	{
		// Otherwise we must be sending to Canvs
		canvas->OnKeyboardButton(key, scancode, action, mods);
	}
}

void GraphManager::OnTextInput(GLFWwindow* window, unsigned int codepoint)
{
	if (activeInterfaceElement)
	{
		// Send Keyboard Event to current active Interface Element
		activeInterfaceElement->OnTextInput((unsigned short*)&codepoint,1);
	}
	else
	{
		// Otherwise we must be sending to Canvs
		canvas->OnTextInput(codepoint);
	}
}

void GraphManager::OnDropWindow(GLFWwindow* window, int count, const char** paths)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (!mainMenu->bHidden && CheckFallIn(mainMenu, xpos, ypos))
	{
		NotifyDropWindow(&mainMenuElements, mainMenu, count, paths, xpos, ypos);
	}
	else if (currentPropertyPanel && !currentPropertyPanel->bHidden && CheckFallIn(currentPropertyPanel, xpos, ypos))
	{
		// Passs to individual elements
		NotifyDropWindow(&propertyPanelElements, currentPropertyPanel, count, paths, xpos, ypos);
	}
	else
	{
		canvas->OnDropWindow(count, paths);
	}
}

void GraphManager::OnResizeFramebuffer(GLFWwindow *window, int width, int height)
{
	windowWidth = width;	// Yes, for us, windowWidth means FrameBuffer width
	windowHeight = height;
	aspectRatio = (float)width / height;

	// Iterate through relavent interfaces and notify them change of framebuffer in case they would like to adjust their viewport
	mainMenu->OnResizeFramebuffer(width, height);
	canvas->OnResizeFramebuffer(width, height - MainMenu::MainMenuHeight);
	docPanel->OnResizeFramebuffer(width, height);
	imgPanel->OnResizeFramebuffer(width, height);
	textPanel->OnResizeFramebuffer(width, height);
	meshPanel->OnResizeFramebuffer(width, height);
}

// OpenGL Error Callback
void APIENTRY GraphManager::OnOpenglError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	cout << "---------------------opengl-callback-start------------" << endl;
	cout << "message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << endl;

	cout << "id: " << id << endl;
	cout << "severity: ";
	//switch (severity){
	//case GL_DEBUG_SEVERITY_LOW:
	//	cout << "LOW";
	//	__debugbreak();
	//	break;
	//case GL_DEBUG_SEVERITY_MEDIUM:
	//	cout << "MEDIUM";
	//	__debugbreak();
	//	break;
	//case GL_DEBUG_SEVERITY_HIGH:
	//	cout << "HIGH";
	//	__debugbreak();	// Used to halt the program so we can solve the error immediately
	//	break;
	//}
	cout << endl;
	cout << "---------------------opengl-callback-end--------------" << endl;
}

// System Interaction
const char* GraphManager::GetClipboardString()
{
	return glfwGetClipboardString(window);
}

void GraphManager::SetClipboardString(char* U8)
{
	glfwSetClipboardString(window, U8);
}

// Static Functions
void GraphManager::UseNewProgram(GLuint newProgram)
{
	if (currentProgram != newProgram)
	{
		glUseProgram(newProgram);
		currentProgram = newProgram;
	}
}
void GraphManager::UseNewTexture0(GLuint texture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);	// Texture Bindings states are irrelavent of programs, but we cannot save much since: 1. when textured being used become a lot, it regardless needs to switch a lot; 
		// 2. Some textures will get deallocated in that edge case if we do not rebound them, they will cause problem(e.g. The original bugs credits to MaterialTexture.cpp texture management)
}
void GraphManager::UseNewTexture1(GLuint texture)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
}
void GraphManager::UseNewTexture2(GLuint texture)
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture);
}
void GraphManager::UseNewTexture3(GLuint texture)
{
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture);
}
void GraphManager::UseNewTexture4(GLuint texture)
{
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texture);
}
void GraphManager::SetCursorIcon(CursorIcon icon)
{
	glfwSetCursor(window, windowCursors[icon]);
}

void GraphManager::UpdateMeshCreationPopup()
{
	// Deprecated
}

// Save to the filename we originally opened
void GraphManager::Save()
{
	// Open the file to write
	FILE* file = fopen(filename, "wb");

	// Write Format Identifier and Document Properties
	fwrite(FORMAT_IDENTIFIER, 3, 1, file);
	int formatVersion = FORMAT_VERSION;
	fwrite(&formatVersion, 1, 1, file);
	unsigned char blockTag = GENERAL_BLOCK_TAG;
	fwrite(&blockTag, sizeof(unsigned char), 1, file);
	fwrite(&createTime, sizeof(Time), 1, file);
	FillWithCurrentTime(&modifyTime);
	fwrite(&modifyTime, sizeof(Time), 1, file);

	// Write Image Data Descriptions
	imageResource->Save(file);

	// Write MEsh Data Descriptions
	meshResource->Save(file);

	// Save Canvas Nodes
	canvas->Save(file);

	// Close the file
	fclose(file);
}

// Notify Window Close
void GraphManager::Close()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

void GraphManager::SwitchFullScreen()
{
	// Do something trivial since we are already in fullscreen upon window creation
	int a = 1;

	// Or do something less trivial
	// Hide Main Menu
	mainMenu->bHidden = !mainMenu->bHidden;
	canvas->OnResizeFramebuffer(windowWidth, mainMenu->bHidden? windowHeight : (windowHeight - MainMenu::MainMenuHeight));

	// Hide Property Panel As well

}

void GraphManager::ShowShortCutsScreen()
{
	unsigned int viewX = windowWidth / 2 - shortcutsScreen->viewportW / 2;
	unsigned int viewY = windowHeight / 2 - shortcutsScreen->viewportH / 2;
	shortcutsScreen->Show(viewX, viewY);
}

void GraphManager::ShowAboutScreen()
{
	unsigned int viewX = windowWidth / 2 - aboutScreen->viewportW / 2;
	unsigned int viewY = windowHeight / 2 - aboutScreen->viewportH / 2;
	aboutScreen->Show(viewX, viewY);
}

void GraphManager::ShowMeshCreationPop()
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	meshPop->Show((unsigned int)xpos, (unsigned int)(windowHeight-ypos-meshPop->viewportH));
}

void GraphManager::ShowTextNodeProperty(TextNode* in)
{
	if(in)textPanel->Show(in);
}

void GraphManager::ShowImageNodeProperty(ImageNode* in)
{
	if(in)imgPanel->Show(in);
}

void GraphManager::ShowMeshNodeProperty(MeshNode* in)
{
	if(in)meshPanel->Show(in);
}

void GraphManager::ShowDocumentProperty()
{
	currentPropertyPanel = NULL;
}

void GraphManager::RegisterMainMenuElements(InterfaceElement* element)
{
	mainMenuElements.Add(element);
}

void GraphManager::RegisterPropertyPanelElements(InterfaceElement* element)
{
	propertyPanelElements.Add(element);
}

void GraphManager::RegisterPopupMenuElements(InterfaceElement* element)
{
	popupMenuElements.Add(element);
}

void GraphManager::RegisterCanvasElements(InterfaceElement* element)
{
	canvasElements.Add(element);

}

// Tools
bool GraphManager::CheckFilename(char* filename)
{
	// Validate whether the filename is recofnized as legal Dream Map files
	// Legal File Names - filename Ending with (Both Upper and Lower Case): .dm .dne .note .dream .dreammap  

	// First Convert filename into upper case
	char* upperCase = (char*)calloc(strlen(filename) + 1, 1);

	// Do Conversion
	for (unsigned int i = 0; i < strlen(filename); i++)
	{
		upperCase[i] = toupper(filename[i]);
	}

	// Do Analysis
	// Get Number of Characters From back of string until first encounter a .
	unsigned int charCount = 0;
	unsigned int filenameLength = strlen(upperCase);
	for (unsigned int i = filenameLength - 1; i >= 0; i--)
	{
		if (upperCase[i] == '.')
		{
			break;
		}
		charCount++;
	}
	// Compare with Existing Know File Suffixes
	switch (charCount)
	{
	case 2:	// .dm - dreamMap
		return (!strcmp(upperCase + filenameLength - charCount, "DM"));
	case 3:	// .dne - dreamEditor
		return (!strcmp(upperCase + filenameLength - charCount, "DNE"));
	case 4:	// .note - dreamNode
		return (!strcmp(upperCase + filenameLength - charCount, "NOTE"));
	case 5:	// .dream - dream
		return (!strcmp(upperCase + filenameLength - charCount, "DREAM"));
	case 8:	// .dreammap - dreamMap
		return (!strcmp(upperCase + filenameLength - charCount, "DREAMMAP"));
	default:
		return false;
	}

	// Free memory
	free(upperCase);
}

bool GraphManager::GetInterfaceMode(char* modeOption)
{
	// Translate string into bool: bFullscreen
	return (!strcmp(modeOption, "true"));
}

void GraphManager::FillWithCurrentTime(Time* timeData)
{
	time_t rawTime;
	struct tm * timeInfo;

	time(&rawTime);
	timeInfo = localtime(&rawTime);	// Return value is an internal pointer, so we do not need to bother

	timeData->day = timeInfo->tm_mday;
	timeData->month = timeInfo->tm_mon + 1;
	timeData->year = timeInfo->tm_year + 1900;
	timeData->second = timeInfo->tm_sec;
	timeData->minute = timeInfo->tm_min;
	timeData->hour = timeInfo->tm_hour;
}

//bool GraphManager::IsImageFile(unsigned short* path, unsigned int numChars)
//{
//	// Validate whether the filename is recofnized as legal Image Files
//	// Legal File Names - filename Ending with (Limited Upper and Lower Case): .png .jpg .bmp
//	// We did it this way to keep the program portable and doesn't rely on STDLIB wstring functions
//
//	// Check Only the last 3 characters
//
//	// Make a copy since we shouldn't maipulate on input string
//	unsigned short suffix[4] = { 0 };
//
//	// Extract Word
//	unsigned int length = GetStringLength(path);
//	for (unsigned int i = 0; i < 3; i++)
//	{
//		suffix[3 -1 - i] = path[length -1 - i];
//	}
//
//	// Compare with Existing Know File Suffixes
//	if (CompareStrings(suffix, (unsigned short*)L"PNG")){ return true; }
//	else if (CompareStrings(suffix, (unsigned short*)L"JPG")){ return true; }
//	else if (CompareStrings(suffix, (unsigned short*)L"BMP")){ return true; }
//	else if (CompareStrings(suffix, (unsigned short*)L"png")){ return true; }
//	else if (CompareStrings(suffix, (unsigned short*)L"jpg")){ return true; }
//	else if (CompareStrings(suffix, (unsigned short*)L"bmp")){ return true; }
//	else{ return false; }
//}
//bool GraphManager::IsMeshSpecFile(unsigned short* path, unsigned int numChars)
//{
//	// Recognized File Format: .ms
//
//	// Make a copy since we shouldn't maipulate on input string
//	unsigned short suffix[4] = { 0 };
//
//	// Extract Word
//	unsigned int length = GetStringLength(path);
//	for (unsigned int i = 0; i < 3; i++)
//	{
//		suffix[3 - 1 - i] = path[length - 1 - i];
//	}
//
//	// Compare with Existing Know File Suffixes
//	if (CompareStrings(suffix, (unsigned short*)L"MS")){ return true; }
//	else if (CompareStrings(suffix, (unsigned short*)L"ms")){ return true; }
//	else{ return false; }
//}
//bool GraphManager::IsDreamMapFile(unsigned short* path, unsigned int numChars)
//{
//	int size = SystemFunctions::U16ToANSI(path, numChars, NULL, NULL);
//
//	char* ANSIPath = (char*)malloc(size + 2);	// Assume ANSI use two bytes for NULL
//	SystemFunctions::U16ToANSI(path, numChars, ANSIPath, size);
//	ANSIPath[size] = 0;
//	ANSIPath[size + 1] = 0;
//
//	bool result = CheckFilename(ANSIPath);
//	delete ANSIPath;
//
//	return result;
//}
bool GraphManager::IsImageFile(char* path)	// ANSI
{
	// Validate whether the filename is recofnized as legal Mesh Spec files
	// Legal File Names - filename Ending with (Both Upper and Lower Case): .png .jpg .bmp

	// First Convert filename into upper case
	char* upperCase = (char*)calloc(strlen(path) + 1, 1);
	for (unsigned int i = 0; i < strlen(path); i++)
	{
		upperCase[i] = toupper(path[i]);
	}

	// Do Analysis
	// Get Number of Characters From back of string until first encounter a .
	unsigned int charCount = 0;
	unsigned int filenameLength = strlen(upperCase);
	for (unsigned int i = filenameLength - 1; i >= 0; i--)
	{
		if (upperCase[i] == '.')
		{
			break;
		}
		charCount++;
	}

	// Compare with Existing Know File Suffixes
	if (!strcmp(upperCase + filenameLength - charCount, "PNG")){ free(upperCase); return true; }
	else if (!strcmp(upperCase + filenameLength - charCount, "JPG")){ free(upperCase); return true; }
	else if (!strcmp(upperCase + filenameLength - charCount, "BMP")){ free(upperCase); return true; }
	else{ free(upperCase); return false; }
}

void GraphManager::GrabCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	bCursorLockedForCamera = true;
}

void GraphManager::ReleaseCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	bCursorLockedForCamera = false;
}

bool GraphManager::IsMeshSpecFile(char* path)	// ANSI
{
	// Validate whether the filename is recofnized as legal Mesh Spec files
	// Legal File Names - filename Ending with (Both Upper and Lower Case): .ms

	// First Convert filename into upper case
	char* upperCase = (char*)calloc(strlen(path) + 1, 1);
	for (unsigned int i = 0; i < strlen(path); i++)
	{
		upperCase[i] = toupper(path[i]);
	}

	// Do Analysis
	// Get Number of Characters From back of string until first encounter a .
	unsigned int charCount = 0;
	unsigned int filenameLength = strlen(upperCase);
	for (unsigned int i = filenameLength - 1; i >= 0; i--)
	{
		if (upperCase[i] == '.')
		{
			break;
		}
		charCount++;
	}

	// Compare with Existing Know File Suffixes
	if (!strcmp(upperCase + filenameLength - charCount, "MS")){ free(upperCase); return true; }
	if (!strcmp(upperCase + filenameLength - charCount, "SPEC")){ free(upperCase); return true; }
	else{ free(upperCase); return false; }
}

bool GraphManager::IsDreamMapFile(char* path)	// ANSI
{
	return CheckFilename(path);
}

void GraphManager::ApplicationLog(char* logString)
{
	char pathBuffer[MAX_PATH];
	FILE* file = fopen(SystemFunctions::GetExecutableDirectoryPath(APPLICATION_LOG_FILE, pathBuffer), "a");
	fprintf(file, "%s\n", logString);
	fclose(file);
}

// Application Flow Functions
void GraphManager::Render()
{
	// Set Fog
	float* skyColor = canvas->GetSkyColor();
	//glClearColor(skyColor[0] + 0.2f, skyColor[1] + 0.2f, skyColor[2] + 0.25f, 1.0f);
	//glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);
	glClearColor(skyColor[0] * 2, skyColor[1] * 2, skyColor[2] * 2, 1.0f);

	// Clear Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//// First Render the Canvas(Canvas will render its interface and 3D Nodes)
	glViewport(canvas->viewportX, canvas->viewportY, canvas->viewportW, canvas->viewportH);
	canvas->Render();

	// For 2D Elements rendering, disable depth test
	glDisable(GL_DEPTH_TEST);

	// Second Render Side Panel Interface
	//if (currentPropertyPanel != NULL)	// Can be null if we are hiding(can be null if nothing is selected)
	//{
	//	glViewport(currentPropertyPanel->viewportX, currentPropertyPanel->viewportY, 
	//		currentPropertyPanel->viewportW, currentPropertyPanel->viewportH);
	//	currentPropertyPanel->Render();
	//}
	//else
	//{
	//	glViewport(docPanel->viewportX, docPanel->viewportY, docPanel->viewportW, docPanel->viewportH);
	//	docPanel->Render();
	//}	// Deprecated now we are using floating panels

	// Render all the panels
	// But I suddenly realized that render all panels in a tilted floating manner isn't possible due to our design of viewport constriant: we 
	//	cannot define a non-rectangular viewport, and InterfaceElements rely on such an Viewport to be correctly rendered. Thus no rotation is
	//	expected in our current design

	// Render only the active panel
	if (mainMenu->bHidden == false)	// If MainMenu is hidden, meaning we should be hidden as well
	{
		if (currentPropertyPanel == currentActivePanel)
		{
			if (currentPropertyPanel == NULL)
			{
				currentPropertyPanel = currentActivePanel = docPanel;
			}

			if (animationScale < 1.0f)
			{
				// Do tranaparency animation
				currentPropertyPanel->Transparency(animationScale);

				// Do movement animation, also render the property panel
				//viewportX + currentPropertyPanel->viewportW * (1 - animationScale)	// Although this effect is cool, but not what we wanted
				glViewport(currentPropertyPanel->viewportX - currentPropertyPanel->viewportW * (1 - animationScale), currentPropertyPanel->viewportY, currentPropertyPanel->viewportW, currentPropertyPanel->viewportH);
				currentPropertyPanel->Render();

				// Proceed Animation
				animationScale += 0.05;
			}
			else
			{
				// Render at destinated position
				glViewport(currentPropertyPanel->viewportX, currentPropertyPanel->viewportY, currentPropertyPanel->viewportW, currentPropertyPanel->viewportH);
				currentPropertyPanel->Render();
			}
		}
		else
		{
			// Hide currentActivePanel to prepare for showing currentPropertyPanel

			// Do tranaparency animation
			currentActivePanel->Transparency(animationScale);

			// Do movement animation, also render the property panel
			glViewport(currentActivePanel->viewportX - currentActivePanel->viewportW * (1 - animationScale), currentActivePanel->viewportY, currentActivePanel->viewportW, currentActivePanel->viewportH);
			currentActivePanel->Render();

			// Proceed Animation
			animationScale -= 0.05;

			if (animationScale < 0.f)
			{
				// Go to another state, show current selected panel
				currentActivePanel = currentPropertyPanel;
			}
		}
	}

	glDisable(GL_DEPTH_TEST);	// Disable Depth Test again since MeshPropertyPanel will enable it due to our current design or TextContainer

	// Third Main Menu Interface
	if (mainMenu->bHidden == false)
	{
		glViewport(mainMenu->viewportX, mainMenu->viewportY, mainMenu->viewportW, mainMenu->viewportH);
		mainMenu->Render();
	}

	// Finally Popup Menu Interface
	if (currentPopup != NULL)
	{
		glViewport(currentPopup->viewportX, currentPopup->viewportY, currentPopup->viewportW, currentPopup->viewportH);
		currentPopup->Render();
	}

	// Revert Settin back
	glEnable(GL_DEPTH_TEST);

	// Swap buffesr
	glfwSwapBuffers(window);
}

// Application Loading and Closing Functions
bool GraphManager::InitializeApplication()
{
	// Initialize Log File
	char pathBuffer[MAX_PATH];
	FILE* file = fopen(SystemFunctions::GetExecutableDirectoryPath(APPLICATION_LOG_FILE, pathBuffer), "w");	// Effectively Clear File
	fprintf(file, "Dream Editor Application Log: \n");
	if (!file)
	{
		__debugbreak();
	}
	fclose(file);

	// Initialize Window, openGL and Interface
	if (!InitailizeWindow())
	{
		return false;
	}

	// Initialize OpenGL
	InitializeOpenGL();

	// Initialize Interface
	InitializeInterface();

	return true;
}

bool GraphManager::InitailizeWindow()
{
	// Currently we are not testing with BackGround Mode

	// Set glfw error callback
	glfwSetErrorCallback(OnWindowError);

	// Initialize GLFW library
	if (!glfwInit())
	{
		cout << "Failed to initialize glfw." << endl;
		return false;
	}

	// Set Context hints
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// Not sure the effect of requring 1.0 while use 4.3 functions?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set window video mode, can make create window faster, can also help prevent "Out of range" when create full screen window
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	desktopWidth = mode->width;
	desktopHeight = mode->height;
	glfwWindowHint(GLFW_DECORATED, false);	// __Solution__: Default Full Screen with decoration-less window; Since Borderless Full screen is just like real fullscreen

#ifdef OPENGL_DEBUG
	// Faciliate openGL debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);	// Enable OpenGL Debug
#endif

	// Create window
	char* titleSuffix = " - Dream Editor";
	char* windowTitle = (char*)calloc(strlen(filename) + strlen(titleSuffix) + 1, 1);
	strcat(windowTitle, filename);
	strcat(windowTitle, titleSuffix);
//#define DEBUG_ORIGINAL_WINDOW	// For inspecting command line output; Otherwise we should Full Screen window for final release
#ifdef DEBUG_ORIGINAL_WINDOW
	// >>Original
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
#else
	// >>Border-less FullScreen(Real-Mode dedicated full screen)
	//windowWidth = desktopWidth;	// Add those two lines since some interface's initialziation depend on those two variables
	//windowHeight= desktopHeight;
	//window = glfwCreateWindow(desktopWidth, desktopHeight, windowTitle, monitor, NULL);	// Obviously this doesn't work
	// >>BackDrop Full Screen(Compatible-Mode full Screen window)
	windowWidth = desktopWidth;
	windowHeight = desktopHeight - 1;	// __Solution__: Default Full Screen with decoration-less window; Use -1 to avoid blinking; 
	window = glfwCreateWindow(desktopWidth, desktopHeight - 1, windowTitle, NULL, NULL);	//	GO directly to full screen size so user don't see (it is in fact)the border-less window(rather than full screen) since anyway we cannot move it
	glfwSetWindowPos(window, 0, 0);	// Move window to the upper left corner.
#endif

	// Check Window Creation status
	if (window == NULL){ glfwTerminate(); return false; }
	free(windowTitle);
	

	// Set glfw window specifc event callback
	glfwSetKeyCallback(window, OnKeyboardButton);
	glfwSetFramebufferSizeCallback(window, OnResizeFramebuffer);
	glfwSetCursorPosCallback(window, OnMouseOver);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	// glfwSetWindowSizeCallback(window, OnResizeWindow);	// Never USed
	glfwSetCharCallback(window, OnTextInput);
	glfwSetDropCallback(window, OnDropWindow);

	// Create some cursors
	CreateWindowCursors();

	// Make the current window's context current openGL context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);	// Vert Sync

	// glew
	glewExperimental = GL_TRUE;	// Handle Vertex Attrib array object creation error: access violation and enable creation of debug context
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << glewGetErrorString(err) << endl;
		return false;
	}

	// Set up openGL debug error callbacks
#ifdef OPENGL_DEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);	// Better than GL_DEBUG_OUTPUT, because the error might come later
	glDebugMessageCallback(OnOpenglError, NULL);
#endif

	return true;
}

void GraphManager::InitializeOpenGL()
{
	// Initialize DevIL, no need to manually close
	// __Debug__: Require better solutions
	// Check Library Version, trivial
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION)
		/*ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)*/
	{
		printf("[Error]DevIL version is different!\n");
	}
	// Initialize iL and iLu, we are not using ilut so no need to initialize
	ilInit();
	iluInit();

	// Shaders
	shaderResource = new ShaderResource();

	// Font
	font = new FontManager();

	// Initial States
	//glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(0.69f, 0.79f, 0.89f, 1.0f);	// Default Fog Color
	glClearDepth(1.f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	// GLobal Debug_Temp
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	// Set by Interface Render() function: glViewport(0, 0, windowWidth, windowHeight);
}

void GraphManager::InitializeInterface()
{
	// Seriously, I am beginning to wonder how long it will take to initialize our application

	// Create Side property panels
	docPanel = new DocumentPropertyPanel(this);
	imgPanel = new ImageNodePropertyPanel();
	textPanel = new TextNodePropertyPanel();
	meshPanel = new MeshNodePropertyPanel();

	// Create main menu bar
	mainMenu = new MainMenu();
	// Create Canvas
	canvas = new Canvas();
	canvas->Initialize();	// Thus when we create canvas objects, the pointer to GraphManager::canvas will be valid at this time
}

void GraphManager::PostInitializeInterface()
{
	// Post some interface initialization here to overcome cross reference order: e.g. FileLoading would require canvas to be initialized while meshResource empty, however PopupMenu would require meshResource be complete

	// Complete All mesh spec files searching and registering
	// meshResource->PostLoad();	// Deprecated: We will not automatically search for more

	// Create Special Display Interfaces
	shortcutsScreen = new ShortcutsPopupScreen();
	aboutScreen = new AboutPopupScreen();

	// Create Popup menus
	filePop = new FileMenuPopupMenu(this);
	windowPop = new WindowMenuPopupMenu(this);
	helpPop = new HelpMenuPopupMenu(this);
	meshPop = new MeshCreationPopupMenu(this);

	// Do not forget our main menu
	mainMenu->RegisterPopups(filePop, windowPop, helpPop);
}

void GraphManager::CreateWindowCursors()
{
	windowCursors[NormalCursor] = NULL;
	windowCursors[TextCursor] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	if (windowCursors[TextCursor] == NULL)
	{
		cout << "[Error] Failed to create cursor! Though doesn't matter much.";
	}
	windowCursors[CanvasCursor] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);;
	if (windowCursors[CanvasCursor] == NULL)
	{
		cout << "[Error] Failed to create cursor! Though doesn't matter much.";
	}

	// Cursors will be destroyed by glfwTerminate()

	// Might want to expose custom cursors creation since we have extra texture space
}

void GraphManager::CloseLibraries()
{
	// OpenGL Resource Deletion has been done by renderables, InterfaceElements and ShaderResource object

	// GLFW
	glfwDestroyWindow(window);
	glfwTerminate();

	// Curtesy... Courtesy
#ifdef APPLICATION_DEBUG
	cin.get(); // Not used for final version since it is a graphical interface
#endif
}

// Interaction Logic Functions
void GraphManager::NotifyMouseOver(LinkedList* elementsList, Interface* parent, double xpos, double ypos)	// Notice ypos is in GLFW space, not in openGl viewoprt space, that is, from UL of the window
{
	// Convert to Interface openGL space
	xpos = xpos - parent->viewportX;
	ypos = (windowHeight - (parent->viewportH + parent->viewportY)) - ypos;

	InterfaceElement* element = (InterfaceElement*)(elementsList->First());
	while (element)	//  element->parent == parent to make sure we check only those associated with current visible property/popup panel
	{
		// Only check if the element belongs to the parent
		if (element->parent == parent)
		{
			if (element->CheckFallIn(xpos, ypos))
			{
				element->OnMouseOver(xpos, ypos);
				// return; // Do not return here to totally avoid edge case when two elements are really close
			}
			else
			{
				if (element->bMouseOverNotified == true)
				{
					element->OnMouseLeave();
				}
			}
		}

		element = (InterfaceElement*)(elementsList->Next());
	}
}

void GraphManager::NotifyMouseOverMainMenu(double xpos, double ypos)
{
	// Convert to Interface openGL space
	xpos = xpos - mainMenu->viewportX;
	ypos = (windowHeight - (mainMenu->viewportH + mainMenu->viewportY)) - ypos;

	// Do normall checking
	InterfaceElement* element = (InterfaceElement*)(mainMenuElements.First());
	while (element)
	{
		if (element->parent == mainMenu)
		{
			if (element->CheckFallIn(xpos, ypos))
			{
				element->OnMouseOver(xpos, ypos);

				// Something Special about mainmenu
				mainMenu->OnElementMouseOver(element);
			}
			else
			{
				if (element->bMouseOverNotified == true)
				{
					element->OnMouseLeave();
				}
			}
		}

		element = (InterfaceElement*)(mainMenuElements.Next());
	}
}

bool GraphManager::CheckFallIn(Interface* check, double xpos, double ypos)
{
	// Convert to Interface openGL space
	xpos = xpos - check->viewportX;
	ypos = (windowHeight - ((double)check->viewportH + check->viewportY)) - ypos;	// Notice ypos is in GLFW space, not in openGl viewoprt space, that is, from UL of the window
	// Use double keyword to force explicit type conversion, otherwise it may cause unsigened integer type overflow

	if (check->CheckFallIn(xpos, ypos))
	{
		return true;
	}
	return false;
}

bool GraphManager::NotifyMouseButton(LinkedList* elementsList, Interface* parent, int button, int action, int mods, double xpos, double ypos)
{
	// Convert to Interface openGL space
	xpos = xpos - parent->viewportX;
	ypos = (windowHeight - ((int)parent->viewportH + (int)parent->viewportY)) - ypos;

	InterfaceElement* element = (InterfaceElement*)(elementsList->First());
	while (element)
	{
		if (element->parent == parent && element->CheckFallIn(xpos, ypos))
		{
			// Notify Interface Element
			element->OnMouseButton(button, action, mods, xpos, ypos);
			// Interface May also be interested when a button is clicked
			parent->OnElementMouseButton(element, button, action, mods);
			// Set Current Active Interface Element
			activeInterfaceElement = element;
			return true;
		}

		element = (InterfaceElement*)(elementsList->Next());
	}

	// If we didn't find any receiving element, reset active element
	activeInterfaceElement = NULL;
	return false;
}

void GraphManager::NotifyDropWindow(LinkedList* elementsList, Interface* parent, int count, const char** paths, double xpos, double ypos)
{
	// Convert to Interface openGL space
	xpos = xpos - parent->viewportX;
	ypos = (windowHeight - ((int)parent->viewportH + (int)parent->viewportY)) - ypos;

	InterfaceElement* element = (InterfaceElement*)(elementsList->First());
	while (dynamic_cast<TextEditor*>(element) != NULL && element->parent == parent)
	{
		if (element->CheckFallIn(xpos, ypos))
		{
			// Notify Interface Element
			element->OnDropWindow(count, paths);
			// Set Current Active Interface Element
			activeInterfaceElement = element;
		}

		element = (InterfaceElement*)(elementsList->Next());
	}

	// If we didn't find any receiving element, reset active element
	activeInterfaceElement = NULL;
}