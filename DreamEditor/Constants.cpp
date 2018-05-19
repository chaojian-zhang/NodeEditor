#include "Constants.h"
#include "Locales.h"

unsigned int InterfaceConstant::DefaultFontSize = 16U;
unsigned int InterfaceConstant::DefaultPixelBorderSize = 1U;
unsigned int InterfaceConstant::DefaultTextFieldPixelWidth = 160U;
unsigned int InterfaceConstant::DefaultTextAreaPixelHeight = 64U;
float InterfaceConstant::PixelToMeterScaleFactor = (float)1 / 256;	// Emperical Value; Mathematical should be 1024

char* ApplicationPath::CanvasFrameImagePath = "Data\\CanvasFrame.png"; // For ".\\Data\\CanvasFrame.png", since GetExecutablePath will not return with a \ symbol 
//char* ApplicationPath::MainMenuFrameImagePath = "Data\\MainMenuFrame.png";
//char* ApplicationPath::PropertyFrameImagePath = "Data\\PropertyFrame.png";
#if defined(Locale_Eng)
char* ApplicationPath::IconImageFilePath = "Data\\Interface_Eng.png"; 
#elif defined(Locale_Chn)
char* ApplicationPath::IconImageFilePath = "Data\\Interface_Chn.png";
#endif
char* ApplicationPath::PrecompiledProgramFilePath = "Data\\Shaders.program";
char* ApplicationPath::CharMapPath = "Data\\CharMap.fnt";	// File name cannot be casually changed since char map file name are encoded inside .fnt binary
char* ApplicationPath::DocumentationManualPath = "Documentation";

// Application Data: Mesh Related Resource do not need to be in specific directory since they will be searched in Executable and Document Data dir
char* ApplicationPath::TerrainMeshSpecMath = "Terrain.spec";
char* ApplicationPath::AnywhereDoorMeshSpecMath = "AnywhereDoor.spec";

// Textures
char* ApplicationPath::DefaultColorTextureMaskPath = "DefaultColorTextureMask.png";
char* ApplicationPath::DefaultSpecularTextureMaskPath = "DefaultSpecularTextureMask.png";
char* ApplicationPath::DefaultEmissiveTextureMaskPath = "DefaultEmissiveTextureMask.png";
char* ApplicationPath::DefaultNormalTexturePath = "DefaultNormalTexture.png";
char* ApplicationPath::DefaultEnvTexturePath= "DefaultEnvBase.jpg";