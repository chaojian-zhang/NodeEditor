#include "RenderableBase.h"
#include <glm\gtc\matrix_transform.hpp>

RenderableBase::RenderableBase()
{
	// Initialize Parameters
	world = glm::mat4(1);
	translation = glm::vec3(0);
	rotation = glm::mat4(1);
	scaling = glm::vec3(1);
}

void RenderableBase::Translation(float x, float y, float z)
{
	translation = glm::vec3(x, y, z);
	UpdateMatrix();
}

void RenderableBase::Rotation(float x, float y, float z)
{
	rotation = glm::rotate(glm::mat4(1), x, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
	UpdateMatrix();
}

void RenderableBase::Scaling(float x, float y, float z)
{
	scaling = glm::vec3(x, y, z);
	UpdateMatrix();
}

void RenderableBase::Translate(float dx, float dy, float dz)
{
	// Update Properties
	translation += glm::vec3(dx, dy, dz);

	// Update Matrix Manually to avoid unncessary update
	// world = glm::translate(world, glm::vec3(dx, dy, dz));
	// This is not a practical short cut since we need to UpdateMatrix() whenever any change is made to our transformation

	UpdateMatrix();
}

void RenderableBase::Rotate(float dx, float dy, float dz)
{
	// Update Properties
	rotation = glm::rotate(rotation, dx, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, dy, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, dz, glm::vec3(0, 0, 1));

	// Update Matrix
	UpdateMatrix();
}

void RenderableBase::Scale(float dx, float dy, float dz)
{
	// Update Properties
	scaling *= glm::vec3(dx, dy, dz);

	// Update Matrix
	UpdateMatrix();
}


void RenderableBase::UpdateMatrix()
{
	// Update Matrix
	world = glm::scale(glm::mat4(1), scaling);	// Scale First
	world = glm::translate(world, translation) * rotation;	// Then rotate, Finally Translate
}