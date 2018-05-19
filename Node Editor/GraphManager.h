#ifndef GRAPHMANGER_H
#define GRAPHMANGER_H
#include "node.h"
#include "InterfaceComponents.h"
#include <stdio.h>	// Provides FILE
#include "include\GLFW\glfw3.h"	// Window creation is handled by the application, but graph manager need this lib to resond to window events

enum GraphType
{
	DreamNoteGraph,

};

struct CalendarTime
{
	unsigned short year;
	unsigned short month;
	unsigned short day;
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
};

class GraphManager
{
public:
	// Interaction Functions, only need to implement Graph Specific functionalities
	// Text Input
	virtual void OnCharInput() = 0;
	// Keyboard
	virtual void OnKeyboardButton(int key, int scancode, int action, int mods) = 0;
	// Mouse
	virtual void OnMouseButton() = 0;
	virtual void OnMouseMove() = 0;
	virtual void ObjectDispatch() = 0;	// Collision Detection

	// Uopdate Functions
	void UpdareTimer(double inTime){ time = inTime; };
	virtual void UpdateInterface() = 0; // Update text fieds stuff
	virtual void UpdateNodes() = 0; // Do animation stuff

	// Render Functions
	virtual void RenderMenuBar() = 0;	// Called Perframe
	virtual void RenderSidePanel() = 0;	// Only when needed
	virtual void RenderCanvas() = 0;	// Only when needed

	// Object Serialization
	virtual void SaveGraph() = 0;
	virtual void LoadGraph(FILE* file) = 0;	// First byte is already read for window creation; We just need to read what is meaning for to us

	// Object Cleanup
	virtual void CleanUp() = 0;
private:
	// Graph Datas, those are things that need to be saved and loaded
	// Graph Type
	GraphType graphType;

	// Graph Metadata
	struct CalendarTime creationTime;
	struct CalendarTime modifyTime;

	// The properties and resources below are graph specific thus defined only when needed
	// Graph Specific Properties
	// Resources
private:
	// Environment Variables, these are things that only used in run-time
	// Main Menu Bar
	Menubar* mainMenubar;
	// Side Panel
	SidePanel* sidePanel;
	// Canvas
	Canvas* canvas;

	// Small Variables
	// Window Style Control
	bool bDecoration;
	// File handle
	FILE* graphFile;
	// Global timer
	double time = 0;

	// Object Tracking
	// Current Selected Nodes
	Node* currentSelection;
	// Current Selected Connection
	Noodle* connection;
};

class FramelessWindow : public GraphManager
{
protected:
	// Window Movement handling
	void WindowMove();
	// Window Resizing handling
	void WindowResize();
private:
	// window states
	double mouseDownX, mouseDownY;
	bool bLMBDown;
};

class FrameedWindow : public GraphManager
{
protected:
	// Window Resizing handling
	void WindowResize();
};

class DreamNoteManager : public FramelessWindow
{

};
#endif
