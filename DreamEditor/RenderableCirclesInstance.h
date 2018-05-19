//#pragma once
//#include "Renderable.h"
//
//// Provided Functions: Instanced rendering of many circles with different location, color, ...
//class RenderableCirclesInstance : public Renderable
//{
//public:
//	RenderableCirclesInstance(float inRadius);
//
//	// Instancing
//	void DataSetup(unsigned int inNmInstances, float* locations, float* colors);	// location and color are 3 component vectors
//	// Notice contrarary to what we normally assign as renderale coordinate, in this case the origin of the renderable is the geometric center, not the bounding box upper left corner
//	void HighlightSelection(unsigned int instanceID, float* inHighlightColor );	// The user need to know which instance he is trying to highlight;
//	// This make this class not so portable, but well customed to this application
//
//	virtual void Render();
//
//private:
//	float numInstances = 0;
//	float radius = 0;
//	float* highlightColor = NULL;
//};