// Environment Setup
#define UNICODE	// Use Unicode natively in the application all over the place
#define WINDOWS_VERSION
#define OPENGL_DEBUG
#define _CRT_SECURE_NO_WARNINGS

// Include Essential External Libraries
#include "include\GL\glew.h"
#include <Dwmapi.h>	// For Window Transparency, used for QQ style graphs
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "include\GLFW\glfw3.h"
#include "include\GLFW\glfw3native.h"

// Link to useful libraries
#pragma comment(lib, "assimp.lib")
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "openGL32.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "Dwmapi.lib")

// Application Headers
#include "Utilitites.h"
#include "GraphManager.h"

// Global Variables
GraphManager* graph;

void OnWindowError(int error, const char* description)
{
	ErrorOutput(description, __LINE__);
}

static void OnKeyboardButton(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Global Shortcuts
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Graph Specific Shotcuts
	graph->OnKeyboardButton(key, scancode, action, mods);
}

// We should notice that cursor position is in client area space, not window space
static void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	// Blobal behaviros
	/*
	if (bLMBDown)	// This way things are more smooth, you can check it
	{
		static int x, y;	// This way things are even theoratically smoother
		glfwGetWindowPos(window, &x, &y);
		glfwSetWindowPos(window, x + xpos - mouse_down_posx, y + ypos - mouse_down_posy);
	}*/

	// Graph Specific behavior

}

static void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	/*
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &mouse_down_posx, &mouse_down_posy);
		bLMBDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		bLMBDown = false;
	}*/
}

/*The size of a framebuffer may change independently of the size of a window, for example if the window is dragged between a regular monitor and a high-DPI one.*/
// We can assume that this function will always be called when a Resize Window event is generated. It will also be called even when no resize window is generated, thus it is more robust for our purporse
void OnResizeFramebuffer(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool ParseWindowStyle(FILE* file)
{
	// Get a byte
	int graphType = fgetc(file);

	switch (graphType)
	{
	default:
		// Set window hints to define window behavior
		glfwWindowHint(GLFW_DECORATED, false);	// No decoration, wakaka, as expected
		glfwWindowHint(GLFW_FLOATING, true);	// Be annoying as Tecent QQ
		// Set Context hints
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// Not sure the effect of requring 1.0 while use 4.3 functions?
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Because i can only use core...

		// Faciliate openGL debugging
		#ifdef OPENGL_DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);	// Enable OpenGL Debug
			// Set up openGL debug error callbacks
		#endif

		// Set window video mode, can make create window faster, can also help prevent "Out of range" when create full screen window
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);	// For transparent window	// Debug
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		break;
	}

	return true;
}

// Test GLFW Program
int main(int argc, char* argv[])	// We are Intentionally reading in ANSI
{
	// Input check
	// Require a file name as input
	if (argc != 2)
	{
		cout << "Please Specify a file name to load graph." << endl;
		cin.get();
		return 0;	// Not exit failure, this is tolerated behavior, and this is in the final product
	}

	// Initialize GLFW library
	if (!glfwInit()){ return -1; }

	// Set glfw error callback
	glfwSetErrorCallback(OnWindowError);

	// Open the file
	FILE* graphFile = fopen(argv[1], "rb");

	// Read from file to determine window style
	bool bRequestTransparent = ParseWindowStyle(graphFile);

	// Create a window
	GLFWwindow* window;
	window = glfwCreateWindow(640, 320, "Node Editor", NULL, NULL);
	if (window == NULL){ glfwTerminate(); return -1; }
	if (bRequestTransparent) 
	{
		// Enable Transparent
		DWM_BLURBEHIND bb = { 0 };
		bb.dwFlags = DWM_BB_ENABLE;
		bb.fEnable = true;
		bb.hRgnBlur = NULL;
		DwmEnableBlurBehindWindow(glfwGetWin32Window(window), &bb);
	}

	// Set glfw window specifc event callback
	glfwSetKeyCallback(window, OnKeyboardButton);
	glfwSetFramebufferSizeCallback(window, OnResizeFramebuffer);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetMouseButtonCallback(window, OnMouseButton);

	// Now we can safely freeconsole, if under windows
	// For other OS we are not willing to deal with hide console for now
#ifdef WINDOWS_VERSION
#ifdef OPENGL_DEBUG	// Obviously if we are still debugging the program we will need a console handy
	FreeConsole();
#endif
#endif // WINDOWS_VERSION

	// Make the current window's context current openGL context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);	// Vert Sync

	// Do other initializations
	// glew initialization
	// glewExperimental = GL_TRUE;	// Handle Vertex Attrib array object creation error: access violation
	if (glewInit()) {
		cout << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
		// Update timer
		graph->UpdareTimer(glfwGetTime());

		// Render Code
		int x, y;
		glfwGetWindowPos(window, &x, &y);
		glClearColor((float)x / 1920, 0, (float)y / 1080, 0.1);
		glClear(GL_COLOR_BUFFER_BIT);

		// Calculation Code

		// Swap buffesr
		glfwSwapBuffers(window);

		// Event handling
		glfwPollEvents();
	}

	// Clean Up application
	fclose(graphFile);
	graph->CleanUp();
	free(graph);

	// Clean up Window
	// Destroy the window
	glfwDestroyWindow(window);
	// Terminate Library
	glfwTerminate();

	return 0;
}