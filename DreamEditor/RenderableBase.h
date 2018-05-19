#pragma once
#include <GL\glew.h>
#include <glm\matrix.hpp>
#include <glm\mat4x4.hpp>

// RenderableBase provides basic definitions for an renderable, namely, transformations. Its children include Renderable only for now. Renderable3DMesh doesn't need a transformation since it is shared
class RenderableBase
{
public:
	RenderableBase();

	// Common Functions: Location in world coordinate
	// void Transformation(float x, float y, float z = 0, float rx = 0, float ry = 0, float rz = 0, float sx = 1, float sy = 1, float sz = 1);	// All are absolute
	void Translation(float x, float y, float z = 0);
	void Rotation(float x, float y, float z = 0);
	void Scaling(float x, float y, float z = 1);
	void Translate(float dx, float dy, float dz = 0);
	void Rotate(float dx, float dy, float dz = 0);
	void Scale(float dx, float dy, float dz = 1);
	// Notice Translate is relative to current position, and subject to rotation and scaling; while translation is relative to object itself, i.e. in object space

protected:
	// Renderable Property
	glm::mat4x4 world;
	// Keep Track of specific properties in order to do things correctly
	glm::vec3 translation;
	glm::mat4x4 rotation;
	glm::vec3 scaling;

	void UpdateMatrix();
};