#include <glm\matrix.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// A Class Describes the World
class World
{
public:
	// World Interface
	World();
	~World();

	float GetSightDepth() { return fogDepth; }

protected:
	// World Properties; Directly Accessible for Child Class
	float fogDepth;	// In meters
	glm::vec3 sunDirection;	// In Degress

private:
	// Default World Configurations; Provides a way to retrieve it back if undergoes dynamic changing
	static float DefaultFogDepth;
	static glm::vec3 DefaultSunDirection;
};

