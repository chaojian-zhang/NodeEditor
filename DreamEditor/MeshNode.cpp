#include "MeshNode.h"
#include "GraphManager.h"

MeshNode::MeshNode(FILE* file)
{
	fread(&Tx, sizeof(float), 1, file);
	fread(&Ty, sizeof(float), 1, file);
	fread(&Tz, sizeof(float), 1, file);
	fread(&rotation, sizeof(glm::mat4x4), 1, file);
	fread(&S, sizeof(float), 1, file);
	fread(&createTime, sizeof(Time), 1, file);

	unsigned short meshIndex;
	fread(&meshIndex, sizeof(unsigned short), 1, file);

	unsigned int commentSize;
	fread(&commentSize, sizeof(unsigned int), 1, file);
	unsigned short* tempComment = (unsigned short*)malloc(commentSize);	// Not Efficient
	fread(tempComment, commentSize, 1, file);

	// Create 
	commentData = new StringData(tempComment, commentSize/2);
	meshSpec = GraphManager::meshResource->GetMesh(meshIndex);
	meshSpec->RequireReference();

	UpdateMatrix();

	// Free resources
	free(tempComment);
}

MeshNode::MeshNode(float x, float y, float z, const char* specFile)
{
	// Create a mesh data
	meshSpec = new MeshSpec(specFile);
	GraphManager::meshResource->AddMesh(meshSpec);
	meshSpec->RequireReference();

	// Metadata
	GraphManager::FillWithCurrentTime(&createTime);

	// Initialize self
	Tx = x;
	Ty = y;
	Tz = z;
	Rx = Ry = Rz = 0;	// Although we initialize it here, we never use Rx,Ry,Rz in our MeshNode
	S = 1;
	commentData = new StringData();

	UpdateMatrix();
}

MeshNode::~MeshNode()
{
	// Delete Datas
	if (commentData)
	{
		delete commentData;
		commentData = NULL;
	}
	// We do not own mesh, so do not delete mesh

	// Inform of Unuses
	if (meshSpec)
	{
		meshSpec->ReleaseReference();
	}
}

// Interface Implementation
void MeshNode::SaveData(FILE* file)
{
	if (bPendingDeletion)	return;

	unsigned char blockTag = MESH_NODE_BLOCK_TAG;
	fwrite(&blockTag, sizeof(unsigned char), 1, file);

	// Save Transformation Information
	fwrite(&Tx, sizeof(float), 1, file);
	fwrite(&Ty, sizeof(float), 1, file);
	fwrite(&Tz, sizeof(float), 1, file);
	fwrite(&rotation, sizeof(glm::mat4x4), 1, file);
	fwrite(&S, sizeof(float), 1, file);
	fwrite(&createTime, sizeof(Time), 1, file);

	// Retrive Information From MeshSpec and save its index. At this time MeshResources should have already been saved, and MeshSpec will have an assigned index of the saved slot
	unsigned short saveSlot = meshSpec->GetSavedSlot();
	fwrite(&saveSlot, sizeof(unsigned short), 1, file);

	// Save Comment
	unsigned int commentSize = commentData->GetStringNumChars() * 2;
	fwrite(&commentSize, sizeof(unsigned int), 1, file);
	fwrite(commentData->GetString(), commentSize, 1, file);
}

void MeshNode::WakeUp()
{
	CanvasNode::WakeUp();

	if (bPendingDeletion)	return;

	// Do not call base wakeUp
	// CanvasNode::WakeUp();

	// Try to wake up the mesh spec
	{
		renderMesh = meshSpec->RequireMeshUsage();
		renderMesh = meshSpec->RequireMeshUsage();	// Attemp it twice in a row in case it loads fast enough; actually we are doing this simply to compensate initial loading camera doesn't show meshes
	}
	if (renderMesh)
	{
		// Set State
		bSleep = false;
	}
	else
	{
		bSleep = true;
	}
	// Otherwise continue be in the state of sleep

	// __Pending: Retrive BBox of the shape
	// Currently we are just faking one
	width = height = depth = 1;
}

void MeshNode::FallAsleep()
{
	CanvasNode::FallAsleep();

	if (bPendingDeletion)	return;

	// Make sure we have at least acquired a mesh by checking we are not asleep
	if (!bSleep)
	{
		meshSpec->ReleaseMeshUsage();
		renderMesh = NULL;
		bSleep = true;
	}
}

void MeshNode::Render()
{
	if (bPendingDeletion)	return;

	if (renderMesh)
	{
		// Render the mesh
		Canvas* canvas = GraphManager::canvas;
		renderMesh->Render(world, canvas->GetViewMatrix(), canvas->GetProjectMatrix());
	}
}

void MeshNode::Translation(float x, float y, float z)
{
	CanvasNode::Translation(x,y,z);
	UpdateMatrix();
}
void MeshNode::Rotation(float x, float y, float z)
{
	CanvasNode::Rotation(x, y, z);
	rotation = glm::rotate(glm::mat4(1), x, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
	UpdateMatrix();
}
void MeshNode::Scaling(float s)
{
	CanvasNode::Scaling(s);
	UpdateMatrix();
}
void MeshNode::Translate(float dx, float dy, float dz)
{
	CanvasNode::Translate(dx, dy, dz);
	UpdateMatrix();
}
void MeshNode::Rotate(float rotdx, float rotdy, float rotdz)
{
	CanvasNode::Rotate(rotdx, rotdy, rotdz);
	rotation = glm::rotate(rotation, rotdx, glm::vec3(1, 0, 0));
	rotation = glm::rotate(rotation, rotdy, glm::vec3(0, 1, 0));
	rotation = glm::rotate(rotation, rotdz, glm::vec3(0, 0, 1));
	UpdateMatrix();
}
void MeshNode::Scale(float ds)
{
	CanvasNode::Scale(ds);
	UpdateMatrix();
}

void MeshNode::UpdateMatrix()
{
	// Update Matrix
	world = glm::scale(glm::mat4(1), glm::vec3(S,S,S));
	world = glm::translate(world, glm::vec3(Tx, Ty, Tz)) * rotation;
}

// INteraction Interface
void MeshNode::OnMouseOver(double xpos, double ypos)
{
	CanvasNode::OnMouseOver(xpos, ypos);

	// Do nothing special
}

void MeshNode::OnMouseButton(int button, int action, int mods, double xpos, double ypos)
{
	CanvasNode::OnMouseButton(button, action, mods, xpos, ypos);

	// Do nothing special
	// In the future we might want to highlight the object border

	GraphManager::ShowMeshNodeProperty(this);
}

void MeshNode::OnKeyboardButton(int key, int scancode, int action, int mods)
{
	CanvasNode::OnKeyboardButton(key, scancode, action, mods);
	
	// Delete MeshNode
	if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
	{
		bPendingDeletion = true;
		FallAsleep();
	}
}

void MeshNode::OnMouseLeave()
{
	CanvasNode::OnMouseLeave();

	// No used
}

unsigned short* MeshNode::GetName()
{
	return meshSpec->GetName();
}

bool MeshNode::RayCollision(glm::vec3 rayDirWorld, glm::vec3 rayLocWorld)
{
	// Get Ray into object space
	glm::vec3 rayDirObj = glm::vec3(glm::inverse(world) * glm::vec4(rayDirWorld, 0));
	glm::vec3 rayLocObj = glm::vec3(glm::inverse(world) * glm::vec4(rayLocWorld, 1));

	// Do Collision Detection
	if (meshSpec->GetPolySoup())
	{
		return meshSpec->GetPolySoup()->CheckRayCollision(rayDirObj, rayLocObj);
	}
	else
	{
		return false;
	}
}