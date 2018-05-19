//#include "Renderable3DTerrain.h"
//#include "GraphManager.h"
//#include "Colors.h"
//
//// Renderabls are positioned by calling UpdateLocation() explicitly, not in the constructor
//Renderable3DTerrain::Renderable3DTerrain()
//	:Renderable(Programs::ShapeProgram, GraphManager::canvas)
//{
//	// Initialize OpenGL obejcts
//	glBindVertexArray(VAO);
//
//	// Generate Vertex Data
//	// Unit in meters; CW order; Notice we are in World Space Now, so up direction is Y
//	float vertices[4 * 3] =
//	{
//		//-65536, 0, -65536,	// __Debug__ Upper Limit of world not Determined
//		//65536, 0, -65536,
//		//65536, 0, 65536,
//		//-65536, 0, 65536
//		// Debug
//		//-1, 0.35, -0.936-0.1,
//		//1, 0.349, -0.936-0.1,
//		//1, -0.345, 0.936-0.1,
//		//-1, -0.349, 0.936-0.1
//		// Sample group 1: perspective matrix specification near/far value is into -Z, so the below will show up while without -5 won't
//		//-1, 0.35, -0.936-5,
//		//1, 0.349, -0.936-5,
//		//1, -0.345, 0.936-5,
//		//-1, -0.349, 0.936-5
//		/* Sample group 2: perspective matrix specification near/far value is into -Z, so the below won't show up when use only perspective*mat4(1)
//		-1, 0.35, -0.936 +5,
//		1, 0.349, -0.936 +5,
//		1, -0.345, 0.936 +5,
//		-1, -0.349, 0.936 +5*/
//		// BTW, the above values are a tilted plane
//		//-1, 1, -0.001f,
//		//1, 1, -0.001f,Z
//		//1, -1, -0.001f,
//		//-1, -1, -0.001f
//		// Horizontal Plane
//		-60000, -0.05, -60000,	// Manually Shift Downward a bit£¬ so when user drop an object resting on height level the bottom of the object has priority to be rendered
//		60000, -0.05, -60000,
//		60000, -0.05, 60000,
//		-60000, -0.05, 60000
//		/*-20, 0, -1000,
//		20, 0, -1000,
//		20, 0, 1000,
//		-20, 0, 1000*/
//		// __Debug__ 65536 cause the object disappear as a whole
//	};
//
//	// Generate Vertex Buffer
//	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
//	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW);	// Always static
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
//	glEnableVertexAttribArray(0);
//}
//
//void Renderable3DTerrain::Render()
//{
//	// Set up states
//	glBindVertexArray(VAO);
//	GraphManager::UseNewProgram(program);
//	// Bind Data, parameters already using openGL coordinates
//	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
//	glUniform3fv(2, 1, Color::TerrainColor);
//	glUniform1f(3, 1.f);
//
//	// Render
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//}