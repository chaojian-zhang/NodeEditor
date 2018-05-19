#include "World.h"

float World::DefaultFogDepth = 25;
glm::vec3 World::DefaultSunDirection = glm::vec3(63.449, -12.359, 17.615);

World::World()
{
	// Properties Initialization
	fogDepth = DefaultFogDepth;
}

World::~World()
{

}