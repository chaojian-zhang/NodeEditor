//#pragma once
//#include "Interface.h"
//#include "Canvas.h"
//#include "MaterialTextures.h"
//#include "WorldMap.h"
//
//class MapEditor : public Interface
//{
//public:
//	// Constructor
//	MapEditor();
//	~MapEditor();
//
//	// Interface Implementation
//	virtual void Render();
//	virtual void OnElementMouseButton(InterfaceElement* element, int button, int action, int mods);
//	virtual void OnResizeFramebuffer(int width, int height);
//
//private:
//	// Configurations
//	static const unsigned int MapEditorWidth;
//	static const unsigned int MapEditorHeight;
//	static const unsigned int HorizontalMargin;
//	static const unsigned int VerticalMargin;
//	static const float MapEditorTexCoord[8];
//
//private:
//	// Interface Elements
//	WorldMap* map = NULL;
//	Button* exportMap = NULL;
//	Panel* background = NULL;
//
//private:
//	// Important Reference
//	Canvas* canvas = NULL;
//
//	// BookKeeping
//	int currentLocationX;	// Implementation of movable MapEditor window
//	int currentLocationY;	// In viewport space, i.e. relative to lower-left corner of window
//};