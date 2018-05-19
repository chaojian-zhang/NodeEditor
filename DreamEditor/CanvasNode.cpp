#include "CanvasNode.h"
#include "Canvas.h"
#include <math.h>

void CanvasNode::OnMouseOver(double xpos, double ypos)
{
	if (bPendingDeletion) return;

	bMouseOverNotified = true;

	// __Pending HighLight Object Rendering
}

// Common Interface
void CanvasNode::Render()
{
	if (bPendingDeletion)	return;

	// Render nodalRenderables
	{
		Renderable* element = (Renderable*)(nodalRenderables.First());
		while (element)
		{
			element->Render();

			element = (Renderable*)(nodalRenderables.Next());
		}
	}

	// Render textContainers
	{
		TextContainer* element = (TextContainer*)(textContainers.First());
		while (element)
		{
			element->Render();

			element = (TextContainer*)(textContainers.Next());
		}
	}
}

void CanvasNode::Translation(float x, float y, float z)
{
	Tx = x;
	Ty = y;
	Tz = z;

	// Children implement their own layout
}

void CanvasNode::Rotation(float x, float y, float z)
{
	// Update Properties
	Rx = x;
	Ry = y; 
	Rz = z;

	// Update nodalRenderables
	{
		Renderable* element = (Renderable*)(nodalRenderables.First());
		while (element)
		{
			element->Rotation(Rx, Ry, Rz);

			element = (Renderable*)(nodalRenderables.Next());
		}
	}

	// Update textContainers
	{
		TextContainer* element = (TextContainer*)(textContainers.First());
		while (element)
		{
			element->Rotation(Rx, Ry, Rz);

			element = (TextContainer*)(textContainers.Next());
		}
	}
}

void CanvasNode::Scaling(float s)
{
	// Update Property
	S = s;

	// Update nodalRenderables
	Renderable* element = (Renderable*)(nodalRenderables.First());
	while (element)
	{
		element->Scaling(S, S, S);

		element = (Renderable*)(nodalRenderables.Next());
	}

	// Update textContainers
	{
		TextContainer* element = (TextContainer*)(textContainers.First());
		while (element)
		{
			element->Scaling(S, S, S);

			element = (TextContainer*)(textContainers.Next());
		}
	}
}

void CanvasNode::Translate(float dx, float dy, float dz)
{
	// Update Properties
	Tx += dx;
	Ty += dy;
	Tz += dz;

	// Update nodalRenderables
	Renderable* element = (Renderable*)(nodalRenderables.First());
	while (element)
	{
		element->Translate(dx, dy ,dz);

		element = (Renderable*)(nodalRenderables.Next());
	}

	// Update textContainers
	{
		TextContainer* element = (TextContainer*)(textContainers.First());
		while (element)
		{
			element->Translate(dx, dy, dz);

			element = (TextContainer*)(textContainers.Next());
		}
	}
}

void CanvasNode::Rotate(float rotdx, float rotdy, float rotdz)
{
	// Update Properties
	Rx += rotdx;
	Ry += rotdy;
	Rz += rotdz;

	// Update nodalRenderables
	Renderable* element = (Renderable*)(nodalRenderables.First());
	while (element)
	{
		element->Rotate(rotdx, rotdy, rotdz);

		element = (Renderable*)(nodalRenderables.Next());
	}

	// Update textContainers
	{
		TextContainer* element = (TextContainer*)(textContainers.First());
		while (element)
		{
			element->Rotate(rotdx, rotdy, rotdz);

			element = (TextContainer*)(textContainers.Next());
		}
	}
}


void CanvasNode::Scale(float ds)
{
	// Update Properties
	S *= ds;

	// Update nodalRenderables
	Renderable* element = (Renderable*)(nodalRenderables.First());
	while (element)
	{
		element->Scale(ds, ds, ds);

		element = (Renderable*)(nodalRenderables.Next());
	}

	// Update textContainers
	{
		TextContainer* element = (TextContainer*)(textContainers.First());
		while (element)
		{
			element->Scale(ds, ds, ds);

			element = (TextContainer*)(textContainers.Next());
		}
	}
}

// Canvas Interface Facility
glm::vec3 CanvasNode::GetWorldPosition()
{
	return glm::vec3(Tx, Ty, Tz);
}

void* CanvasNode::operator new (size_t size)
{
	// __Debug__: Might not be portable/working
	return _aligned_malloc(size, 16);
}

void CanvasNode::operator delete (void *p)
{
	_aligned_free(p);
}