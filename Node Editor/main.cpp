// Application Libraries x32bit:
//	GLFW, GLEW, GLM
//	AssImp, DevIL, freetype

// Notice:
// glfw thread safety: http://www.glfw.org/docs/latest/intro.html

// Environment Setup
#define UNICODE	// Use Unicode natively in the application, but use ANSI for command line debug output
#define DEBUG_MODE true
// Include Useful libraries
#include "include\GL\glew.h"
// Debug
// #include <Dwmapi.h>
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "include\GLFW\glfw3.h"
#include "include\GLFW\glfw3native.h"
// #include "include\GLFW\glfw3.h"
// Native libraries
#include <iostream>
#include <stdlib.h>
#include <intrin.h>
using std::cin;
using std::cout;
using std::endl;

// Link to useful libraries
#pragma comment(lib, "assimp.lib")
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "openGL32.lib")
#pragma comment(lib, "glfw3dll.lib")
// Debug
// #pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "User32.lib")
/*assimp.lib
DevIL.lib
ILU.lib
ILUT.lib
freetype.lib
glew32.lib
openGL32.lib
glfw3dll.lib*/

// Global Variables
// Engine Specific
double time = 0;
// Main window states
double mouse_down_posx, mouse_down_posy;
bool bLMBDown;

// Utilities
void ErrorOutput(char* string, int line_nb)	// string here only for ANSI console output
{
	cout << "Error: " << string << "< At: Line " << line_nb << ">" << endl;
	__debugbreak();
	exit(EXIT_FAILURE);
}

void OnWindowError(int error, const char* description)
{
	cout << description << endl;
}

static void OnKeyboardButton(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// We should notice that cursor position is in client area space, not monitor space
static void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	// if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	if (bLMBDown)	// This way things are more smooth, you can check it
	{
		static int x, y;	// This way things are even theoratically smoother
		glfwGetWindowPos(window, &x, &y);
		glfwSetWindowPos(window, x + xpos - mouse_down_posx, y + ypos - mouse_down_posy);
	}
}

static void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &mouse_down_posx, &mouse_down_posy);
		bLMBDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		bLMBDown = false;
	}
}

/*The size of a framebuffer may change independently of the size of a window, for example if the window is dragged between a regular monitor and a high-DPI one.*/
void OnResizeFramebuffer(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void OnResizeWindow(GLFWwindow *window, int w_width, int w_height)
{
	/*
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);*/// NO need to do this here because OnResizeFramebuffer() will also be called
}

// Action Functions
/* Seemly useless and not necessary
GLFWwindow* GetMainWindow(GLFWwindow* window = NULL)	// This function exists that so we do not define a global window pointer, because I just don't feel like doing that
{
	static GLFWwindow* mainWindow;
	if (window != NULL)
	{
		mainWindow = window;
	}
	return mainWindow;
}

void MoveWindow(int x, int y)	// Move main window
{
	glfwSetWindowPos(GetMainWindow(), x, y);
}

void MoveWindowdt(int dx, int dy)	// dt stands for delta
{
	int x, y;
	glfwGetWindowPos(GetMainWindow(), &x, &y);
	glfwSetWindowPos(GetMainWindow(), x + dx, y + dy);
}*/

void Init()
{
	// glew initialization
	// glewExperimental = GL_TRUE;	// Handle Vertex Attrib array object creation error: access violation
	if (glewInit()) {
		cout << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
}

// Test GLFW Program
int main()	// For debug, we should use main, not wmain() because we need to output ANSI to the console, otherwise on ANSI cmd consoles, we will not be able to display UNICODE without setting environ locale, and even if we do so, i doubt the user might have unicode lanuguage pack, but anyway we will only output ANSI to the console
// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)	// For no console window, we use wWinMain() here because we are not outputing to the console
{
	// Create a window
	GLFWwindow* window;

	// Initialize GLFW library
	if (!glfwInit()){ return -1; }

	// Set glfw error callback
	glfwSetErrorCallback(OnWindowError);

	// Set window hints to define window behavior
	glfwWindowHint(GLFW_DECORATED, false);	// No decoration, wakaka, as expected
	glfwWindowHint(GLFW_FLOATING, true);	// Be annoying as Tecent QQ
	// Set Context hints
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// Not sure the effect of requring 1.0 while use 4.3 functions?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Because i can only use core...

	// Faciliate openGL debugging
	if (DEBUG_MODE)
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);	// Enable OpenGL Debug
		// Set up openGL debug error callbacks
	}

	// Set window video mode, can make create window faster, can also help prevent "Out of range" when create full screen window
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);	// For transparent window	// Debug
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	// Create window
	window = glfwCreateWindow(640, 320, "Node Editor", NULL, NULL);
	if (window == NULL){ glfwTerminate(); return -1; }
	// (Experimental)Application Specifi Function: Set hidden global window pointer
	// GetMainWindow(window);

	/*
	// Debug: Enable Transparent
	DWM_BLURBEHIND bb = { 0 };
	bb.dwFlags = DWM_BB_ENABLE;
	bb.fEnable = true;
	bb.hRgnBlur = NULL;
	DwmEnableBlurBehindWindow(glfwGetWin32Window(window), &bb);*/
	SetWindowLongPtr(glfwGetWin32Window(window), GWL_EXSTYLE, WS_EX_ACCEPTFILES | WS_EX_LAYERED);// | WS_EX_TRANSPARENT);

	// Set glfw window specifc event callback
	glfwSetKeyCallback(window, OnKeyboardButton);
	glfwSetFramebufferSizeCallback(window, OnResizeFramebuffer);
	glfwSetWindowSizeCallback(window, OnResizeWindow);
	glfwSetCursorPosCallback(window, OnMouseMove);
	glfwSetMouseButtonCallback(window, OnMouseButton);

	// Make the current window's context current openGL context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);	// Vert Sync

	// Do other initializations
	Init();

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
		// Get current time
		time = glfwGetTime();

		// Render Code
		int x, y;
		glfwGetWindowPos(window, &x, &y);
		glClearColor((float)x / 1920, 0, (float)y / 1080, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Calculation Code

		// Swap buffesr
		glfwSwapBuffers(window);

		// Event handling
		glfwPollEvents();
	}

	// Destroy the window
	glfwDestroyWindow(window);

	// Terminate Library
	glfwTerminate();

	return 0;
}