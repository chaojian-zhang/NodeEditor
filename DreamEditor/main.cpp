/* Hard Constriants:
	1. Interface using openGL second quadrant coordinate system, and this is also the internal coordiantes used
	2. Support only unsigned short charset because we are providing charmap containing only those much glyphs
*/
/* Program Behavior:
	1. By design any esthetically appealing program mush have animations, even advanced skeleton animation are not supported due to not
	useful in our case but shader animation is perfectly legit; To balance between resource comsumption and
	actual usage, the application will only do endless loop when it is focuses, when out of focus, the applicatoin will halt all animation
	rendering.
*/
#include "GraphManager.h"
#include <fstream>

#pragma comment(lib, "assimp.lib")
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")	// Contains debug facilities
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "openGL32.lib")
#pragma comment(lib, "Shlwapi.lib")	// For PathFileExist


// Use Input arguments to specify default loading map and initial interface mdoe -- Used for Drag and Load and File Opening and Startup Desktop
// Command line format: [ProgramName] [Filename] [InterfaceMode]
int main(int argc, char* argv[])
{
	char* fileName = nullptr;
	char* interfaceMode = "default";

	// If no file was provided, create one
	if (argc == 1)
	{
		fileName = "default.dream";
		std::ofstream{ fileName };
	}
	if (argc == 2)
		fileName = argv[1];
	if (argc == 2)
	{
		fileName = argv[1];
		interfaceMode = argv[2];
	}
	
	// Create a Graph
	GraphManager graph(fileName, interfaceMode);

	if (graph.bInitializeSuccess)
	{
		// Enter Main loop
		graph.Begin();
	}
	else
	{
		cout << "Initialization failure." << endl;
		cin.get();
	}

	return 0;
}



/* Program Usage and Logic Flow
	1. Graph File Creation and Loading:
		The Program Cannot be executed alone, because it doesn't and we will not provide direct file creation facilities.
		The user hereby must manually create a blank .dream(all lower/uppper case variation: .dreammap, .dm, .graph, .note, .dne) file, and specify it
		as command line argument into the executable.
		A third parameter can be used to indicate whether the program should run in FullScren background mode.
		"true" dedicated full screen and hiding mainmenu and side panel are toggled in in-application menu and shortcuts.
	2. 
*/