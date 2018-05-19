//#include "MapEditor.h"
//#include "SystemWrapper.h"
//#include "Constants.h"
//#include "GraphManager.h"
//
//const unsigned int MapEditor::MapEditorWidth = 850U;
//const unsigned int MapEditor::MapEditorHeight = 500U;
//const unsigned int MapEditor::HorizontalMargin = 32U;
//const unsigned int MapEditor::VerticalMargin = 32U;
//const float MapEditor::MapEditorTexCoord[8] = {
//	0.0f / 2048.f, 1198.f / 2048.f,
//	500.f / 2048.f, 2048.f / 2048.f,
//	500.f / 2048.f, 1198.f / 2048.f,
//	0.0f / 2048.f, 2048.f / 2048.f };
//
//
//MapEditor::MapEditor()
//{
//	// Create Background Texture
//	GLuint texture = MaterialTextures::GetTexture(SystemFunctions::GetExecutableDirectoryPath(ApplicationPath::IconImageFilePath));
//
//	// Create Interface Elements
//	exportMap = new Button((unsigned short*)Interface_MapEditor_ExportMap, this);
//	background = new Panel(MapEditorWidth, MapEditorHeight, MapEditorTexCoord, texture, this);
//	map = new WorldMap(MapEditorWidth - HorizontalMargin * 2, ShadowMapHeight - VerticalMargin * 2 - exportMap->GetDimensionY(), this);
//
//	// Do manual layout
//	background->UpdatePosition(0, 0);
//	exportMap->UpdatePosition(MapEditorWidth - HorizontalMargin - exportMap->GetDimensionX(), -(MapEditorHeight - VerticalMargin - exportMap->GetDimensionY()));
//	map->UpdatePosition(HorizontalMargin, - VerticalMargin);
//
//	// Interface Properties
//	bHidden = false;
//	currentLocationX = GraphManager::windowWidth / 2 - MapEditorWidth /2;
//	currentLocationY = GraphManager::windowHeight / 2 + MapEditorHeight / 2;
//	OnResizeFramebuffer(GraphManager::windowWidth, GraphManager::windowHeight);
//
//	// Register Input Interaction with Graph Manager
//	GraphManager::RegisterMainMenuElements(map);
//	GraphManager::RegisterMainMenuElements(exportMap);
//}
//
//MapEditor::~MapEditor()
//{
//	delete map;
//	delete background;
//	delete exportMap;
//}
//
//void MapEditor::Render()
//{
//	if (!bHidden)
//	{
//		// Render background first
//		background->Render();
//		exportMap->Render();
//		map->Render();
//	}
//}
//
//void MapEditor::OnElementMouseButton(InterfaceElement* element, int button, int action, int mods)
//{
//	if (element == exportMap)
//	{
//		// Do some export
//	}
//}
//
//void MapEditor::OnResizeFramebuffer(int width, int height)
//{
//	// Notice these are relative to Lower-Left corner of the window
//	viewportX = currentLocationX;
//	viewportY = currentLocationY;
//	viewportW = MapEditorWidth;
//	viewportH = MapEditorHeight;
//
//	// Update Projection and View
//	UpdateProjection();
//}