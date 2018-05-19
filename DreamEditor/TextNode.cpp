#include "TextNode.h"
#include <stdio.h>
#include "Colors.h"
#include "GraphManager.h"
// Include Headers where we need them
// e.g. stdio, math.h

// Static Variables
float TextNode::XPadding = 0.1f;
float TextNode::YPadding = 0.1f;
float TextNode::MinWidth = 1.f;	// In meter
float TextNode::MinHeight = 0.8f;
float TextNode::MaxWidth = 2.5f;
float TextNode::MaxHeight = 1.8f;
unsigned int TextNode::FontSize = 16U;
float TextNode::QuoteCornerSize = 0.1f;
float TextNode::FieldWidth = 1.5f;
//float TextNode::TextFrameColor[3] = { 0.788f, 0.788f, 0.788f };
//float TextNode::TextFrameHoverColor[3] = { 0.f, 0.48f, 0.769f };
//float TextNode::TextBackgroundColor[3] = { 0.23f, 0.23f, 0.23f };
//float TextNode::SurfaceBorderColor[3] = { 0.788f, 0.788f, 0.788f };
//float TextNode::SurfaceColor[3] = { 0.407f, 0.407f, 0.407f };

//float TextNode::TextFrameColor[3] = { 0.233f, 0.233f, 0.233f };
//float TextNode::TextFrameHoverColor[3] = { 0.f, 0.48f, 0.769f };
////float TextNode::TextBackgroundColor[3] = { 0.366f, 0.366f, 0.366f };
//float TextNode::TextBackgroundColor[3] = { 0.f, 0.f, 0.f};
//float TextNode::SurfaceBorderColor[3] = { 0.f, 0.f, 0.f };
//float TextNode::SurfaceColor[3] = { 0.412f, 0.412f, 0.412f };

float TextNode::TextFrameColor[3] = { 0.f, 0.184f, 0.02f};
float TextNode::TextFrameHoverColor[3] = { 0.f, 0.48f, 0.769f };
float TextNode::TextBackgroundColor[3] = { 0.05f, 0.082f, 0.f}; 
float TextNode::SurfaceBorderColor[3] = { 0.05f, 0.246f, 0.f};
float TextNode::SurfaceColor[3] = { 0.266f, 0.559f, 0.242f };
// Color Testing can use Photoshop

float TextNode::SurfaceOpacity = 0.75f;

TextNode::TextNode(FILE* file)
{
	fread(&Tx, sizeof(float), 1, file);
	fread(&Ty, sizeof(float), 1, file);
	fread(&Tz, sizeof(float), 1, file);
	fread(&Rx, sizeof(float), 1, file);
	fread(&Ry, sizeof(float), 1, file);
	fread(&Rz, sizeof(float), 1, file);
	fread(&S, sizeof(float), 1, file);
	fread(&createTime, sizeof(Time), 1, file);

	unsigned int titleSize;
	fread(&titleSize, sizeof(unsigned int), 1, file);
	unsigned short* tempTitle = (unsigned short*)malloc(titleSize);	// Not Efficient
	fread(tempTitle, titleSize, 1, file);

	unsigned int contentSize;
	fread(&contentSize, sizeof(unsigned int), 1, file);
	unsigned short* tempContent = (unsigned short*)malloc(contentSize);	// Not Efficient
	fread(tempContent, contentSize, 1, file);

	// Create Data
	titleData = new StringData(tempTitle, titleSize /2);
	contentData = new StringData(tempContent, contentSize /2);

	// Free resources
	free(tempTitle);
	free(tempContent);
}

TextNode::TextNode(float x, float y, float z)
{
	Tx = x;
	Ty = y;
	Tz = z;
	Rx = Ry = Rz = 0;
	S = 1;
	GraphManager::FillWithCurrentTime(&createTime);

	// Create Data
	titleData = new StringData();
	contentData = new StringData();
}

TextNode::~TextNode()
{
	delete titleData;
	delete contentData;

	if(frame)delete frame;
	if(titleField)delete titleField;
	if (contentField)delete contentField;
	if(title)delete title;
	if(content)delete content;
}

// Interface Implementation
void TextNode::Update()
{
	if (bPendingDeletion)	return;

	// Resize Animation
	if (bResizing)
	{
		// Profiling: Test How many frames it take to extend width of one character(approxi. 3cm width)
		static int FrameNumber = 0;
		FrameNumber++;	// around 45 frames

		// The bigger the difference, the faster the increase speed: exponential growth with carrying capacity, or https://en.wikipedia.org/wiki/Logistic_function
		// f(x) = 1/(1+e^((-10)(4x-0.15)))  -- see wolframalpha; accuracy: 3cm; Increamentation: maximum 6cm; Minimum 0.2 percent increase
		currentWidth += 1. / (1. + glm::pow<double>(glm::e<double>(), ((-10.)*(4.*(width - currentWidth) - 0.5)))) * 0.06;	// Notice here I change middle point to 0.5 to make minimum increase smaller
		currentHeight += 1. / (1. + glm::pow<double>(glm::e<double>(), ((-10.)*(4.*(height - currentHeight) - 0.5)))) * 0.06;

		// End Case: End Case must be before clamping otherwise cannot work well
		if (currentWidth >= width && currentHeight >= height)
		{
			bResizing = false;
		}

		// Clamping
		if (currentWidth > width)
		{
			currentWidth = width;
		}
		if (currentHeight > height)
		{
			currentHeight = height;
		}

		// Update Node Elements
		float textAreaCornerSize = title->GetBoundaryHeight() / 4;
		//titleField->UpdateBuffer(title->GetBoundaryWidth(), title->GetBoundaryHeight()*1.02, textAreaCornerSize);
		//contentField->UpdateBuffer(content->GetBoundaryWidth(), height - YPadding * 3 - title->GetBoundaryHeight()/*Do not use content->GetBoundaryHeight(), since for MinHeight case those two doesn't equal*/, textAreaCornerSize);
		//frame->UpdateBuffer(width, height, QuoteCornerSize);
		titleField->UpdateBuffer(currentWidth - XPadding * 2, title->GetBoundaryHeight()*1.02, textAreaCornerSize);
		contentField->UpdateBuffer(currentWidth - XPadding * 2, currentHeight - YPadding * 3 - title->GetBoundaryHeight(), textAreaCornerSize);
		frame->UpdateBuffer(currentWidth, currentHeight, QuoteCornerSize);
	}
}

void TextNode::SaveData(FILE* file)
{
	if (bPendingDeletion)	return;

	unsigned char blockTag = TEXT_NODE_BLOCK_TAG;
	fwrite(&blockTag, sizeof(unsigned char), 1, file);
	fwrite(&Tx, sizeof(float), 1, file);
	fwrite(&Ty, sizeof(float), 1, file);
	fwrite(&Tz, sizeof(float), 1, file);
	fwrite(&Rz, sizeof(float), 1, file);
	fwrite(&Ry, sizeof(float), 1, file);
	fwrite(&Rz, sizeof(float), 1, file);
	fwrite(&S, sizeof(float), 1, file);
	fwrite(&createTime, sizeof(Time), 1, file);
	unsigned int titleSize = titleData->GetStringNumChars() * 2;
	fwrite(&titleSize, sizeof(unsigned int), 1, file);
	fwrite(titleData->GetString(), titleSize, 1, file);
	unsigned int contentSize = contentData->GetStringNumChars() * 2;
	fwrite(&contentSize, sizeof(unsigned int), 1, file);
	fwrite(contentData->GetString(), contentSize, 1, file);
}

void TextNode::WakeUp()
{
	CanvasNode::WakeUp();

	if (bPendingDeletion)	return;
	
	// Create Title First and Get Dimensions
	//unsigned short* debugTitleText = (unsigned short*)L"Debug Title: This is a very long debug title to make sure autoFitting and MouseHover is working correctly";
	//title = new TextContainer(GraphManager::canvas, debugTitleText, GetStringLength(debugTitleText), FontSize, Color::DefaultTextColor, 0, 0, true, true, true, titleData);
	// title = new TextContainer(GraphManager::canvas, (unsigned short*)L"Debug Title", 11, FontSize, Color::DefaultTextColor, 0, 0, true, true, true, titleData);
	title = new TextContainer(GraphManager::canvas, titleData->GetString(), titleData->GetStringNumChars(), FontSize, Color::DefaultTextColor, 0, 0, true, true, true, false, false, titleData);
	width = title->GetBoundaryWidth() > MinWidth ? title->GetBoundaryWidth() : MinWidth;

	// Create Content
	// unsigned short* debugConentText = (unsigned short*)L"Debug Content: It was once said that there was a dragon living in the deepest ground¡£ÔÚºÜÓÐºÜ¾ÃÒÔÇ°ÊÀ½çÉÏÖ»ÓÐÊ¯Í·Ã»ÓÐÊ÷¡£";
//	unsigned short* debugConentText = (unsigned short*)
//L"¤à¤«¤·¡¢ ¤à¤«¤·¡¢ ¤¢¤ë ¤È¤³¤í ¤Ë\n\
//¤ª¤¸¤¤¤µ¤ó ¤È ¤ª¤Ð¤¢¤µ¤ó ¤¬ ¤¤¤Þ¤·¤¿¡£\n\
//¤ª¤¸¤¤¤µ¤ó ¤¬ É½£¨¤ä¤Þ£© ¤Ø Ä¾£¨¤­£© ¤ò ¤­¤ê ¤Ë ¤¤¤±¤Ð¡¢\n¤ª¤Ð¤¢¤µ¤ó ¤Ï ´¨£¨¤«¤ï£© ¤Ø ¤»¤ó¤¿¤¯ ¤Ë ¤Ç¤«¤±¤Þ¤¹¡£\n\
//¡¸¤ª¤¸¤¤¤µ¤ó¡¢ ¤Ï¤è¤¦ ¤â¤É¤Ã¤Æ ¤­¤Ê¤µ¤ì¡£¡¹\n¡¸¤ª¤Ð¤¢¤µ¤ó ¤â ¤­ ¤ò ¤Ä¤±¤Æ ¤Ê¡£¡¹	\n\
//¤Þ¤¤ÈÕ£¨¤Ë¤Á£© ¤ä¤µ¤·¤¯ ¤¤¤¤ ¤¢¤Ã¤Æ ¤Ç¤«¤±¤Þ¤¹¡£";
//	content = new TextContainer(GraphManager::canvas, debugConentText, GetStringLength(debugConentText), FontSize, Color::DefaultTextColor, width, 0, false, true, true, contentData);
	// content = new TextContainer(GraphManager::canvas, (unsigned short*)L"Debug Content", 13, FontSize, Color::DefaultTextColor, width, 0, false, true, true, contentData);
	content = new TextContainer(GraphManager::canvas, contentData->GetString(), contentData->GetStringNumChars(), FontSize, Color::DefaultTextColor, width, 0, false, true, true, true, false, contentData);
	float elementsHeight = title->GetBoundaryHeight() + YPadding*3 + content->GetBoundaryHeight();
	currentHeight = height = elementsHeight > MinHeight ? elementsHeight : MinHeight;

	// Then Create Conents and other decorations
	float textAreaCornerSize = title->GetBoundaryHeight() / 4;
	//titleField = new RenderableRoundCornerFrame(width, title->GetBoundaryHeight()*1.02, TextFrameColor, textAreaCornerSize, true);	// Yes, Dynamic
	titleField = new RenderableRoundCornerQuad(GraphManager::canvas, width, title->GetBoundaryHeight()*1.02, true, TextFrameColor, TextBackgroundColor, textAreaCornerSize, true, true);	// Yes, Dynamic
	contentField = new RenderableRoundCornerQuad(GraphManager::canvas, width, height - YPadding*3 - title->GetBoundaryHeight(), true, TextFrameColor, TextBackgroundColor, textAreaCornerSize, true, true);
	frame = new Renderable3DQuoteFrame(width + XPadding * 2, height, true, SurfaceBorderColor, SurfaceColor, QuoteCornerSize, true, SurfaceOpacity);	// Yes, Dynamic

	// Update Final Width and Height
	currentWidth = width = width + XPadding * 2;
//	height = (height + YPadding * 2)*1.26;	// Quote Symbol
	// We didn't do this so notice that TextNode's height doesn't take its quote symbol into account

	// Place Renderables
	Translation(Tx, Ty, Tz);

	// Add to render list, in order of rendering
	nodalRenderables.Add(frame);
	nodalRenderables.Add(titleField);
	nodalRenderables.Add(contentField);
	textContainers.Add(title);
	textContainers.Add(content);
}

void TextNode::FallAsleep()
{
	if (bSleep) return;
	bSleep = true;

	if (bPendingDeletion)	return;

	// Delete Renderables
	delete frame;
	frame = NULL;
	delete titleField;
	titleField = NULL;
	delete contentField;
	contentField = NULL;
	delete title;
	title = NULL;
	delete content;
	content = NULL;

	// Clear Render List
	nodalRenderables.Clear();
	textContainers.Clear();
}

void TextNode::Translation(float x, float y, float z)
{
	CanvasNode::Translation(x, y, z);

	frame->Translation(x, y, z);
	title->Translation(x + XPadding + 0.01, y - YPadding, z + 0.003);	// Offset in Z depth a bit to make it not collide; Offset in X a bit so that vertical cursor position bar can be seen
	titleField->Translation(x + XPadding, y - YPadding, z + 0.002);
	content->Translation(x + XPadding + 0.01, y - YPadding - title->GetBoundaryHeight() - YPadding, z + 0.003);// Offset in Z depth a bit to make it not collide; Offset in X a bit so that vertical cursor position bar can be seen
	contentField->Translation(x + XPadding, y - YPadding - title->GetBoundaryHeight() - YPadding, z + 0.002);
}

// INteraction Interface
void TextNode::OnMouseOver(double xpos, double ypos)
{
	CanvasNode::OnMouseOver(xpos, ypos);

	if (bPendingDeletion)	return;

	// Update Texture fields frame rendering highlight effect
	// Here Check Order Matters
	if (xpos > XPadding && 
		xpos < XPadding + width - XPadding * 2 &&
		ypos < -YPadding - title->GetBoundaryHeight() - YPadding && 
		ypos > -(height - YPadding))
	{
		GraphManager::SetCursorIcon(CursorIcon::TextCursor);
		contentField->UpdateProperty(true, TextFrameHoverColor, TextBackgroundColor);
		//titleField->UpdateProperty(TextFrameColor);
		titleField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);

		// Also update TextContainer if necessary
		if (currentSelection == content)
		{
			currentSelection->OnMouseOver(xpos - XPadding, ypos + YPadding + title->GetBoundaryHeight() + YPadding);
		}
	}
	else if (xpos > XPadding && 
		xpos < XPadding + width - XPadding * 2 &&
		ypos < -YPadding && 
		ypos > -YPadding - title->GetBoundaryHeight())
	{
		GraphManager::SetCursorIcon(CursorIcon::TextCursor);
		contentField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);
		//titleField->UpdateProperty(TextFrameHoverColor);
		titleField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);

		// Also update TextContainer if necessary
		if (currentSelection == title)
		{
			currentSelection->OnMouseOver(xpos - XPadding, ypos + YPadding);
		}
	}
	else
	{
		OnMouseLeave();
	}
}
void TextNode::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	CanvasNode::OnMouseButton(button, action, mods, xpos, ypos);

	if (bPendingDeletion)	return;

	if (xpos > XPadding &&
		xpos < XPadding + width - XPadding * 2 &&
		ypos < -YPadding - title->GetBoundaryHeight() - YPadding &&
		ypos > -(height - YPadding))
	{
		currentSelection = content;
		currentSelection->OnMouseButton(button, action, mods, xpos - XPadding, ypos + YPadding + title->GetBoundaryHeight() + YPadding);
	}
	else if (xpos > XPadding &&
		xpos < XPadding + width - XPadding * 2 &&
		ypos < -YPadding &&
		ypos > -YPadding - title->GetBoundaryHeight())
	{
		currentSelection = title;
		currentSelection->OnMouseButton(button, action, mods, xpos-XPadding, ypos+YPadding);
	}
	else
	{
		currentSelection = NULL;
	}

	GraphManager::ShowTextNodeProperty(this);
}
void TextNode::OnKeyboardButton(int key, int scancode, int action, int mods)
{
	// Summon Active TextContainers to do the work
	if (currentSelection)
	{
		// If we are Tabbing in Title bar, then switch focus to 
		//if (currentSelection == title && key == GLFW_KEY_TAB && action == GLFW_PRESS)
		//{
		//	currentSelection = content;
		//}
		//else
		//{
			currentSelection->OnKeyboardButton(key, scancode, action, mods);

			// Since the size of the TextContainer might change, we need to update size of drawings
			AnimatedResize();
		//}
	}
	else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		bPendingDeletion = true;
		FallAsleep();
	}
}
void TextNode::OnTextInput(unsigned short* texts, unsigned int numChars)
{
	if (bPendingDeletion)	return;

	// Add Texts to the Active TextContainer
	if (currentSelection)
	{
		currentSelection->OnTextInput(texts, numChars);

		// Since the size of the TextContainer might change, we need to update size of drawings
		AnimatedResize();
	}
}

void TextNode::OnMouseLeave()
{
	CanvasNode::OnMouseLeave();

	if (bPendingDeletion)	return;

	contentField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);
	//titleField->UpdateProperty(TextFrameColor);
	titleField->UpdateProperty(true, TextFrameColor, TextBackgroundColor);
	GraphManager::SetCursorIcon(CursorIcon::NormalCursor);
}

void TextNode::OnDeselection()
{
	if (currentSelection)
	{
		currentSelection->SpecifyCursorRender(false);
	}
	currentSelection = NULL;
}

// Extra Functionalities
void TextNode::Export()
{
	if (bPendingDeletion)	return;

	char filename[MAX_PATH];
	SystemFunctions::CreateFilename(titleData->GetString(), titleData->GetStringNumChars(), filename);

	// Save Comment
	contentData->Export(filename, &createTime);	// StringData will append a suffix to the file
}

void TextNode::AnimatedResize()
{
	// __Debug__ Might want to add an animation to make the transition smooth with some dragging effect

	// Logic: if title width is bigger than previous state, resize width of all elements; if height of content is bigger than previous case, resize all related elements
	// Lock flexible resizing when hit MaxWidth or MaxHeight

	// Check Newest Width of Title String
	if (title->GetBoundaryWidth() > width - XPadding * 2)
	{
		// Get Newest Width
		float candidateWidth = title->GetBoundaryWidth() + 2 * XPadding;
		width = Maxf(Minf(candidateWidth, MaxWidth), MinWidth);

		// If width exceeds maximum, constraint it
		if (title->GetBoundaryWidth() > MaxWidth - XPadding * 2)
		{
			title->UpdateBoundary(MaxWidth - XPadding * 2, 0, true, false);	// Make it fixed width now
		}
		// Otherwise we do not need to do anything since Title text is flexible width: it will update itself

		// Update Content Text Boundary
		content->UpdateBoundary(title->GetBoundaryWidth(), 0, true, false);

		bResizing = true;
		Update();
	}

	// Check Height of the Content String
	if (content->GetBoundaryHeight() > height -YPadding * 3 - title->GetBoundaryHeight())
	{
		// Get Newest Height
		float candidateHeight = title->GetBoundaryHeight() + YPadding * 3 + content->GetBoundaryHeight();
		height = Maxf(Minf(candidateHeight, MaxHeight), MinHeight);

		// If width exceeds maximum, constraint it
		if (currentSelection->GetBoundaryHeight() > MaxHeight - YPadding * 3 - title->GetBoundaryHeight())
		{
			content->UpdateBoundary(width - XPadding * 2, MaxHeight - YPadding * 3 - title->GetBoundaryHeight(), true, true);
		}
		// Otherwise we do not need to do anything since Title text is flexible height: it will update itself

		bResizing = true;
		Update();
	}
}

unsigned short* TextNode::GetCreateTimeString()
{
	return createTime.GetString();
}