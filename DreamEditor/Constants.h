#pragma once

class InterfaceConstant
{
public:
	static unsigned int DefaultFontSize;
	static unsigned int DefaultPixelBorderSize;
	static unsigned int DefaultTextFieldPixelWidth;
	static unsigned int DefaultTextAreaPixelHeight;
	static float PixelToMeterScaleFactor;
};

class ApplicationPath
{
public:
	static char* CanvasFrameImagePath;
	static char* IconImageFilePath;
	static char* PrecompiledProgramFilePath;
	static char* CharMapPath;
	static char* DocumentationManualPath;

	// Application Data
	static char* TerrainMeshSpecMath;
	static char* AnywhereDoorMeshSpecMath;

	// Textures
	static char* DefaultColorTextureMaskPath;
	static char* DefaultSpecularTextureMaskPath;
	static char* DefaultEmissiveTextureMaskPath;
	static char* DefaultNormalTexturePath;
	static char* DefaultEnvTexturePath;
};

