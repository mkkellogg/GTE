#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <unordered_map>
#include <functional>
#include <vector>

#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/engineobject.h"
#include "object/engineobjectmanager.h"
#include "mesh3D.h"
#include "submesh3D.h"
#include "global/global.h"
#include "ui/debug.h"


Mesh3D::Mesh3D(unsigned int subMeshCount)
{
	if(subMeshCount<=0)subMeshCount=1;
	this->subMeshCount = subMeshCount;
	subMeshes = NULL;
}

Mesh3D::~Mesh3D()
{
	SAFE_DELETE(subMeshes);
}

unsigned int Mesh3D::GetSubMeshCount()
{
	return subMeshCount;
}

bool Mesh3D::Init()
{
	subMeshes = new SubMesh3D*[subMeshCount];
	NULL_CHECK(subMeshes," Mesh3D::Init -> Could not allocate sub meshes.",false);

	return true;
}

void Mesh3D::Update(SubMesh3D * subMesh)
{

}

void Mesh3D::Update()
{

}

void Mesh3D::SetSubMesh(SubMesh3D * mesh, unsigned int index)
{
	NULL_CHECK_RTRN(mesh,"Mesh3D::SetSubMesh -> mesh is NULL.");

	if(index < subMeshCount)
	{
		subMeshes[index] = mesh;
		mesh->SetContainerMesh(this);
	}
	else
	{
		Debug::PrintError("Mesh3D::SetSubMesh -> Index out of range.");
		return;
	}
}

SubMesh3D * Mesh3D::GetSubMesh(unsigned int index)
{
	if(index < subMeshCount)
	{
		return subMeshes[index];
	}
	else
	{
		Debug::PrintError("Mesh3D::GetSubMesh -> Index out of range.");
		return NULL;
	}
}
