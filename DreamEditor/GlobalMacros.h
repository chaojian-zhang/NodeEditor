#pragma once
// Application Meta Information
#define FORMAT_IDENTIFIER "DNE"
#define FORMAT_VERSION 1
#define GENERAL_BLOCK_TAG 1	// Use a macro to make it more obvious in the code
#define IMAGE_DATA_BLOCK_TAG 2
#define MESH_DATA_BLOCK_TAG 3
#define CAMERA_NODE_BLOCK_TAG 4
#define IMAGE_NODE_BLOCK_TAG 5
#define TEXT_NODE_BLOCK_TAG 6
#define MESH_NODE_BLOCK_TAG 7
#define APPLICATION_LOG_FILE "log.txt"
#ifdef _DEBUG	// Visual Studio Debug Switch
	#define OPENGL_DEBUG
#endif

// Global Debug Switch, since we do not know and do not want to depend on VisualStudio specific Solution Configuration Macros _DEBUG and _RELEASE
// Notice this switch is indepent on Visual Studio Configuration Mode since we might want to include some application specific debug outputs in even the release version Solution Configuration
// #define APPLICATION_DEBUG
// For final release, just comment the above line
/*Application Debug Features:
	1. Console is displayed
*/
