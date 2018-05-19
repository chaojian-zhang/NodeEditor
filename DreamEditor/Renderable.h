#pragma once
#include "RenderableBase.h"
#include "Shaders.h"

class Interface;

// Derived Renderable can have their own member vairables, but notice that we want to only keep those that are used to change openGL states or shader uniforms
// __Debug__ In the future, to make things faster and more patchable, we might provide a RenderResource Manager to do all those things in a more optimized way: e.g. generate a bunch of VAOs instead of each one, and render in batch
class Renderable: public RenderableBase
{
public:
	Renderable(Programs inProgram, Interface* parent);	// Require Nothing as input
	~Renderable();

	// Interfaces: Only render is mandatory to implement by children
	// If a derived class would require more variables to define render states, it should define member variables and use member functions to set up those variables
	virtual void Render() = 0;

	// For Transformable and Dynamic Objects, they will provide their own updateBUffer
	// For configurable objects, they will provide updateProperty functions
	// The usage case of those are specific so no need to provide generic interface requirement

	// alignment
	static void* operator new (size_t size);
	static void operator delete (void *p);

protected:
	GLuint VAO;
	GLuint vBuffer;
	GLuint program;	// Each Derived Class constructor will specify their required program, of course also overwrite their appropriate Render() routine

protected:
	Interface* parentInterface;
};