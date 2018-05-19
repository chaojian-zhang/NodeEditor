#pragma once
// Runtime Componnets
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>	// For windows specific functions, need to include this before glfw
#include <gl\glew.h>
#include <GLFW\glfw3.h>
#include <IL\il.h>
#include <IL\ilu.h>
// Application Components
#include "GlobalMacros.h"
#include "Interface.h"
#include "ShaderResource.h"
#include "ImageResource.h"
#include "MeshResource.h"
#include "FontManager.h"
#include "Time.h"
#include "Interfaces.h"
#include "MaterialTextures.h"
// Standard Libraries
#include <glm\matrix.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

class TextNode;
class ImageNode;
class MeshNode;

enum CursorIcon
{
	NormalCursor,
	TextCursor,
	CanvasCursor,
	NumCursors
};

class GraphManager
{
public: 
	GraphManager(char* fn, char* im); // Load file and create interface
	~GraphManager(); // Release Dynamically created resources and close the window, do other clean up as well
	
	// Enter main loop and process events
	void Begin();

	// Event Handling: Basically dispatch all messages to interface objects and interface will dispatch further to interface elements
	static void OnWindowError(int error, const char* description);
	// Mouse
	static void OnMouseOver(GLFWwindow* window, double xpos, double ypos);	// If popup menu is visible, then remember to call related mouse over functionalities, and if they are visible while mouse is well outside them, disable(hide) it
	static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
	// Keyboard and Text
	static void OnKeyboardButton(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void OnTextInput(GLFWwindow* window, unsigned int codepoint);
	// Facility
	static void OnDropWindow(GLFWwindow* window, int count, const char** paths);
	// Window and Framebuffer
	static void OnResizeFramebuffer(GLFWwindow *window, int width, int height);	// Dispath each interface to adjust their viewport position
	// static void OnResizeWindow(GLFWwindow *window, int w_width, int w_height);	// May want to record window width and height here->Record for Framebuffer size // Not Used
	// openGL Error Callabck
	static void APIENTRY OnOpenglError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	// System Intreaction
	static const char* GetClipboardString();
	static void  SetClipboardString(char* U8);
	static void FillWithCurrentTime(Time* timeData);
	static void GrabCursor();	// USed by Canvas to implement camera movement
	static void ReleaseCursor();

	// Files Interaction
	//static bool IsImageFile(unsigned short* path, unsigned int numChars);	// U16
	//static bool IsMeshSpecFile(unsigned short* path, unsigned int numChars);	// U16
	//static bool IsDreamMapFile(unsigned short* path, unsigned int numChars);	// U16
	static bool IsImageFile(char* path);	// ANSI
	static bool IsMeshSpecFile(char* path);	// ANSI
	static bool IsDreamMapFile(char* path);	// ANSI

	// Application Utility
	static void ApplicationLog(char* logString);	// Seems not useful if we can use __debugbreak properly

public:
	bool bInitializeSuccess = false;	// If any reason caused initialize failed, we will directly output to console, main() will just check and execute the program when this boolean is true

	// Document Properties
	static Time createTime;	// Creation
	static Time modifyTime;	// Last save

public:
	// Application Specific Resources
	static ShaderResource* shaderResource;
	static ImageResource* imageResource;
	static MeshResource* meshResource;
	static FontManager* font;

public:
	// Bookeeping use, statically accessble for all objects
	static double systemTime;
	static double deltaTime;	// Time elapse since last frame
	static long frame;
	static int windowWidth, windowHeight;
	static int desktopWidth, desktopHeight;
	static float aspectRatio;
	static GLuint currentProgram;
	
	static GLFWcursor* windowCursors[NumCursors];

	// Functions
	static void UseNewProgram(GLuint newProgram);
	static void UseNewTexture0(GLuint texture);
	static void UseNewTexture1(GLuint texture);
	static void UseNewTexture2(GLuint texture);
	static void UseNewTexture3(GLuint texture);
	static void UseNewTexture4(GLuint texture);
	static void SetCursorIcon(CursorIcon icon);
	static void UpdateMeshCreationPopup();	// Called By Mesh Resource when a New Mesh is added in run-time

	// Bookkeeping use, for application interaction
	static InterfaceElement* activeInterfaceElement;	// Current Active Interface Element, used to send keyboard events
	static Interface* currentPopup;
	static PropertyPanel* currentPropertyPanel;	// Current Selection Property with respect to selected object type in Canvas
	static PropertyPanel* currentActivePanel;	// Current Rendering Property Panel that might still be in animation transition
	static float animationScale;

public:
	// Kernel Functions that might be used by InterfaceElements as well
	static void Save();	// Save to a file, the file is alread specifed when initialize the application, by design we shoudn't need to save into a different file name
	static void Close();
	static void SwitchFullScreen();	// Used by WindowMenu
	void ShowShortCutsScreen();	// USed by HelpMenu
	void ShowAboutScreen();		// USed by HelpMenu
	static void ShowMeshCreationPop();		// USed by Canvas, since canvas doesn't have a pointer to graph object, we need to make it static
	static void ShowTextNodeProperty(TextNode* in);		// Used by Canvas
	static void ShowImageNodeProperty(ImageNode* in);	// Used by Canvas
	static void ShowMeshNodeProperty(MeshNode* in);		// Used by Canvas
	static void ShowDocumentProperty();		// Used by Canvas and ourselves

public:
	// Interfaces
	static MainMenu* mainMenu;
	static Canvas* canvas;
	// Property Panel
	static DocumentPropertyPanel* docPanel;
	static ImageNodePropertyPanel* imgPanel;
	static TextNodePropertyPanel* textPanel;
	static MeshNodePropertyPanel* meshPanel;
	// Popup Menu
	static FileMenuPopupMenu* filePop;
	static WindowMenuPopupMenu* windowPop;
	static HelpMenuPopupMenu* helpPop;
	static MeshCreationPopupMenu* meshPop;
	static ShortcutsPopupScreen* shortcutsScreen;
	static AboutPopupScreen* aboutScreen;

	// Interface ElementManagement
	static void RegisterMainMenuElements(InterfaceElement* element);
	static void RegisterPropertyPanelElements(InterfaceElement* element);
	static void RegisterPopupMenuElements(InterfaceElement* element);
	static void RegisterCanvasElements(InterfaceElement* element);

private:
	// Environment Specific Objects
	//bool bFullscreenBack = false;	// Serve as a desktop background
	static bool bFullscreen;
	static char* filename;	// ANSI filename
	static GLFWwindow* window;

	static LinkedList mainMenuElements;
	static LinkedList propertyPanelElements;
	static LinkedList popupMenuElements;
	static LinkedList canvasElements;

	// Bookeeping
	static bool bCursorLockedForCamera;

private: // Tools
	static bool CheckFilename(char* filename);	// Validate whether the filename is recofnized as legal Dream Map files; ANSI
	bool GetInterfaceMode(char* modeOption);	// Translate string into bool: bFullscreenBack

private:
	// Application Flow Functions
	void Render();	// No need to create specialized Renderer object(e.g. RenderManager or Render Engine Component) since our render tasks are pretty linear and predictable

	// Application Loading and Closing Specifics
	bool InitializeApplication();
	bool InitailizeWindow();
	void InitializeOpenGL();
	void InitializeInterface();
	void PostInitializeInterface();
	void CreateWindowCursors();
	void CloseLibraries();

	// Interaction Logic Specifics
	static void NotifyMouseOver(LinkedList* elementsList, Interface* parent, double xpos, double ypos);	// xpos and ypos in Viewpor Coord Space(LL Window Corner)
	static void NotifyMouseOverMainMenu(double xpos, double ypos);	// Specially handle main menu to better control popup display
	static bool NotifyMouseButton(LinkedList* elementsList, Interface* parent, int button, int action, int mods, double xpos, double ypos);	// Return whether any interface element is clicked; Useful for canvas
	static void NotifyDropWindow(LinkedList* elementsList, Interface* parent, int count, const char** paths, double xpos, double ypos);
	static bool CheckFallIn(Interface* check, double xpos, double ypos);
	// static void GetMouseInterfaceLocation(Interface* parent, double* xpos, double* ypos);	// Return GLFW Window spapce cursor converted to Interface Space Viewport OpenGL Second
	// Since this function is called a lot, it is better to manually inline it instead of using a function to avoid penalty
};