//#include "SingleImage.h"
//#include "GraphManager.h"
//#include "MathElegent.h"
//#include "Interface.h"
//
//float SingleImage::imageCoords[8] = { 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0 };
//
//SingleImage::SingleImage(float width, float height, GLuint texture, Interface* inParent)
//{
//	parent = inParent;
//
//	image = new RenderableImageRect(parent, width, height, imageCoords, texture);
//
//	// Set Width and Height
//	width = width;
//	height = height;
//}
//
//SingleImage::~SingleImage()
//{
//	delete image;
//}
//
//// Interface Implementation
//void SingleImage::UpdatePosition(int newx, int newy)
//{
//	InterfaceElement::UpdatePosition(newx, newy);
//
//	image->Translation(x, y, 0.1);
//}
//
//
//void SingleImage::OffsetPosition(int dx, int dy, float dz)
//{
//	InterfaceElement::OffsetPosition((float)dx, (float)dy, dz);
//
//	image->Translate(dx, dy, dz);
//}
//
//void SingleImage::Render()
//{
//	image->Render();
//}