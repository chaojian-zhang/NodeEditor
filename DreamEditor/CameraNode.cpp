#include "CameraNode.h"
#include "GraphManager.h"

float CameraNode::CameraHeight = 2.0f;

CameraNode::CameraNode(FILE* file)
{
	fread(&Tx, sizeof(float), 1, file);
	fread(&Ty, sizeof(float), 1, file);
	fread(&Tz, sizeof(float), 1, file);
	fread(&Rx, sizeof(float), 1, file);
	fread(&Ry, sizeof(float), 1, file);
	fread(&Rz, sizeof(float), 1, file);
	fread(&S, sizeof(float), 1, file);

	canvas = GraphManager::canvas;

	// Update Rotation Matrix
	Rotation(Rx, Ry, Rz);
	// Initialize view and project matrix
	UpdateMatrix();
}

CameraNode::CameraNode()
{
	// In meter; In World space: Y is up direction
	Tx = Tz = Rx = Ry = Rz = 0;
	Ty = CameraHeight;

	canvas = GraphManager::canvas;

	// Initialize view and project matrix
	UpdateMatrix();
}

//CameraNode::CameraNode(float x, float y, float z, glm::mat4x4* rotation)
//{
//	// __pending
//}

void CameraNode::SaveData(FILE* file)
{
	unsigned char blockTag = CAMERA_NODE_BLOCK_TAG;
	fwrite(&blockTag, sizeof(unsigned char), 1, file);
	fwrite(&Tx, sizeof(float), 1, file);
	fwrite(&Ty, sizeof(float), 1, file);
	fwrite(&Tz, sizeof(float), 1, file);
	fwrite(&Rx, sizeof(float), 1, file);
	fwrite(&Ry, sizeof(float), 1, file);
	fwrite(&Rz, sizeof(float), 1, file);
	fwrite(&S, sizeof(float), 1, file);
}

void CameraNode::Rotate(float rotdx, float rotdy, float rotdz)
{
	CanvasNode::Rotate(rotdx, rotdy, rotdz);

	rotation = glm::rotate(glm::mat4(1), Ry, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), Rx, glm::vec3(1, 0, 0));	// Rotate the camear
	// Notice the order here determines the correct result
}

void CameraNode::Rotation(float x, float y, float z)
{
	CanvasNode::Rotation(x, y, z);

	// Update Rotation Matrix
	rotation = glm::rotate(glm::mat4(1), x, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
}

void CameraNode::UpdateMatrix()
{
	view = glm::inverse(glm::translate(glm::mat4(1), glm::vec3(Tx, Ty, Tz))*rotation);
	project = glm::perspective(/*FOV_degree*/45.f, (float)(canvas->viewportW)/(canvas->viewportH), 0.1f, 500.f);	// Notice +Z is toward the viewer, or into the screen -> This is wrong;	 The smaller zFar/zNear, the bigger the accuracy, thus bigger zNear helps
	// For not obvious reasons, this values seems to specify the range into -Z direction, when no view matrix is used
	// Debug
	// project = glm::perspective(/*FOV_degree*/45.f, (float)(canvas->viewportW) / (canvas->viewportH), -0.936f, 0.936f);	// -> Not render anything even z within range[-1,1]
	// +- seems not affecting at all
	// Indeed: According here: https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml 
	/*
	zNear
	Specifies the distance from the viewer to the near clipping plane
	(always positive).
	zFar
	Specifies the distance from the viewer to the far clipping plane
	(always positive).
	*/
	// -> This also tells us we should carefully read legacy openGL functions for documentation about those GLM functions.
}