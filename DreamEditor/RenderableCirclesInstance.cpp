//#include "RenderableCirclesInstance.h"
//#include "GraphManager.h"
//#include <math.h>
//
//RenderableCirclesInstance::RenderableCirclesInstance(float inRadius)
//	:Renderable(Programs::InstancedCircles, GraphManager::canvas), radius(inRadius)
//{
//	// Do nothing
//}
//
//void RenderableCirclesInstance::DataSetup(unsigned int inNumInstances, float* locations, float* colors)
//{
//	numInstances = inNumInstances;
//
//	// Generate circle vertices
//	#define MAX_NUM_CIRCLE_VERTS 36
//	float vertices[MAX_NUM_CIRCLE_VERTS * 3];
//	for (unsigned int i = 0; i < MAX_NUM_CIRCLE_VERTS; i++)
//	{
//		float angleInRadians = 2 * 3.14159 / MAX_NUM_CIRCLE_VERTS * i;
//		vertices[i] = radius * sin(angleInRadians);
//		vertices[i + 1] = radius * cos(angleInRadians);
//		vertices[i + 2] = 0.f;
//	}
//
//	// Generate shape positions
//	float* shapePositions = (float*)malloc(sizeof(float) * inNumInstances * 3);
//	for (unsigned int i = 0; i < inNumInstances; i++)
//	{
//
//	}
//
//	// Generate colors
//	float* colors = (float*)malloc(sizeof(float) * inNumInstances * 3);
//	for (unsigned int i = 0; i < inNumInstances; i++)
//	{
//
//	}
//}
//
//void RenderableCirclesInstance::HighlightSelection(unsigned int instanceID, float* inHighlightColor)
//{
//
//}
//
//void RenderableCirclesInstance::Render()
//{
//
//}
//
//
//
//static const GLfloat square_vertices[] =
//{
//	-1.0f, -1.0f, 0.0f, 1.0f,
//	1.0f, -1.0f, 0.0f, 1.0f,
//	1.0f, 1.0f, 0.0f, 1.0f,
//	-1.0f, 1.0f, 0.0f, 1.0f
//};
//static const GLfloat instance_colors[] =
//{
//	1.0f, 0.0f, 0.0f, 1.0f,
//	0.0f, 1.0f, 0.0f, 1.0f,
//	0.0f, 0.0f, 1.0f, 1.0f,
//	1.0f, 1.0f, 0.0f, 1.0f
//};
//static const GLfloat instance_positions[] =
//{
//	-2.0f, -2.0f, 0.0f, 0.0f,
//	2.0f, -2.0f, 0.0f, 0.0f,
//	2.0f, 2.0f, 0.0f, 0.0f,
//	-2.0f, 2.0f, 0.0f, 0.0f
//};
//GLuint offset = 0;
//glGenVertexArrays(1, &square_vao);
//glGenBuffers(1, &square_vbo);
//glBindVertexArray(square_vao);
//glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
//glBufferData(GL_ARRAY_BUFFER,
//	sizeof(square_vertices) +
//	sizeof(instance_colors) +
//	sizeof(instance_positions), NULL, GL_STATIC_DRAW);
//glBufferSubData(GL_ARRAY_BUFFER, offset,
//	sizeof(square_vertices),
//	square_vertices);
//offset += sizeof(square_vertices);
//glBufferSubData(GL_ARRAY_BUFFER, offset,
//	sizeof(instance_colors), instance_colors);
//offset += sizeof(instance_colors);
//glBufferSubData(GL_ARRAY_BUFFER, offset,
//	sizeof(instance_positions), instance_positions);
//offset += sizeof(instance_positions);
//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,
//	(GLvoid *)sizeof(square_vertices));
//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0,
//	(GLvoid *)(sizeof(square_vertices) +
//	sizeof(instance_colors)));
//glEnableVertexAttribArray(0);
//glEnableVertexAttribArray(1);
//glEnableVertexAttribArray(2);
//Listing 7.9: Getting ready for instanced rendering
//248 Chapter 7 : Vertex Processing and Drawing Commands
//Now all that remains is to set the vertex attrib divisors for the
//instance_color and instance_position attribute arrays :
//glVertexAttribDivisor(1, 1);
//glVertexAttribDivisor(2, 1);
//Now we draw four instances of the geometry that we previously put into
//our buffer.Each instance consists of four vertices, each with its own
//position, which means that the same vertex in each instance has the same
//position.However, all of the vertices in a single instance see the same
//value of instance_color and instance_position, and a new value of
//each is presented to each instance.Our rendering loop looks like this:
//static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//glClearBufferfv(GL_COLOR, 0, black);
//glUseProgram(instancingProg);
//glBindVertexArray(square_vao);
//glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);