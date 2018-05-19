//#include "TextBox.h"
//#include "GraphManager.h"
//
//// Static Variables
//int TextBox::CornerSize = 2;
//unsigned int TextBox::XPadding = 2;
//unsigned int TextBox::YPadding = 2;
//float TextBox::FrameColor[3] = {0.2,0.2,0.2};
//float TextBox::FrameHoverColor[3] = { 0.9, 0., 0.9 };
//float TextBox::BackGroundColor[3] = {0.222,0.222, 0.222};
//
//// __Debug__
//// Notice lots of code are identical to that of search field. We may need a more condense way to do these things: put it a parent virtual, or make it a common base function.
//
//TextBox::TextBox(Interface* inParent, unsigned short* initialString, unsigned int numChars)
//	:TextField(inParent, initialString, numChars)
//{
//	// Set Correct Width and Height of interface element
//	width = DisplayWidth + XPadding * 2;
//	height = FontSize + YPadding * 2;
//
//	// Initialize REnderable
//	frame = new Renderable2DRoundCornerQuad(width, height, true, FrameColor, BackGroundColor, CornerSize, false, inParent);
//}
//
//TextBox::~TextBox()
//{
//	delete frame;
//}
//
//// Interface modification
//void TextBox::UpdatePosition(int newx, int newy)
//{
//	textRenderable->UpdateLocation(newx + XPadding + textHoriScroll, newy - (int)YPadding, 0);
//	cursor0->UpdateLocationDelta(newx - x, newy - y, 0);
//	cursor1->UpdateLocationDelta(newx - x, newy - y, 0);
//	//cursor0->UpdateLocation((float)(GetInfoAt(cursorPosition0)), (float)(y), 0);
//	//cursor1->UpdateLocation((float)(GetInfoAt(cursorPosition1)), (float)(y), 0);
//	frame->UpdateLocation(newx, newy, 0);
//
//	x = newx;
//	y = newy;
//}
//
//// Render Request
//void TextBox::Render()
//{
//	// Render Frame First
//	frame->Render();
//
//	// Then Render Text
//	// Limit Range of String Renderable display
//	// -> Just Use viewport to limit display range isn't gonnad work, since our project2DView matrix depend on the viewport
//	// First Hack Renderbale View Matrix
//	glm::mat4x4 temp = parent->project2DView;	// Keep a backup
//	glm::mat4x4 translation = glm::translate<float>(glm::mat4(1), glm::vec3(-(int)(x  + XPadding), 0, 0));
//	parent->project2DView = glm::ortho<float>(0, DisplayWidth, -(int)parent->viewportH, 0, 0, 100) * GraphManager::View2D;
//	parent->project2DView *= translation;	// By the way no need to keep the above lines a seperate class member, since it won't take too much computing power while keep the code clean in a sense that those things do not get spreaded all over the class source code
//	// Set Viewport to Limited Range
//	glViewport(parent->viewportX + x + XPadding, parent->viewportY, DisplayWidth, parent->viewportH);	// Limit Horizontal Range of rendering to the text field
//	// Do the render
//	textRenderable->Render();
//	// Set Hacking Back
//	parent->project2DView = temp;
//	// Set Viewport back
//	glViewport(parent->viewportX, parent->viewportY, parent->viewportW, parent->viewportH);
//
//	// Render Cursors
//	cursor0->Render();
//	cursor1->Render();
//}
//
//// Mouse Event handlers
//void TextBox::OnMouseOver(double xpos, double ypos)
//{
//	bMouseOverNotified = true;
//
//	// Change Mouse Cursor
//	GraphManager::SetCursorIcon(CursorIcon::TextCursor);
//
//	// Change Border Color
//	frame->UpdateProperty(true, FrameHoverColor, BackGroundColor);
//}
//
//void TextBox::OnMouseLeave()
//{
//	GraphManager::SetCursorIcon(CursorIcon::NormalCursor);
//	frame->UpdateProperty(true, FrameColor, BackGroundColor);
//
//	bMouseOverNotified = false;
//}