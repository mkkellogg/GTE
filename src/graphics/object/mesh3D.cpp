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
#include "graphics/render/mesh3Drenderer.h"
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
	if(subMeshes != NULL)
	{
		for(unsigned int i =0; i < subMeshCount; i++)
		{
			SAFE_DELETE(subMeshes[i]);
		}
		SAFE_DELETE(subMeshes);
	}
}

unsigned int Mesh3D::GetSubMeshCount()
{
	return subMeshCount;
}

bool Mesh3D::Init()
{
	subMeshes = new SubMesh3D*[subMeshCount];
	NULL_CHECK(subMeshes," Mesh3D::Init -> Could not allocate sub meshes.",false);

	memset(subMeshes, 0, sizeof(SubMesh3D*) * subMeshCount);

	return true;
}

void Mesh3D::Update(SubMesh3D * subMesh)
{
	NULL_CHECK_RTRN(subMesh," Mesh3D::Update -> subMesh is NULL.");

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		if(subMeshes[i] == subMesh)
		{
			if(sceneObject != NULL)
			{
				Mesh3DRenderer * renderer = sceneObject->GetRenderer3D();
				NULL_CHECK_RTRN(renderer," Mesh3D::Update -> renderer is NULL.");

				renderer->UpdateFromMesh(i);
			}
		}
	}
}

void Mesh3D::Update()
{
	NULL_CHECK_RTRN(subMeshes," Mesh3D::Update -> subMeshes is NULL.");

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		if(subMeshes[i] != NULL)
		{
			subMeshes[i]->Update();
		}
	}
}

void Mesh3D::SetSubMesh(SubMesh3D * mesh, unsigned int index)
{
	NULL_CHECK_RTRN(mesh,"Mesh3D::SetSubMesh -> mesh is NULL.");
	NULL_CHECK_RTRN(subMeshes,"Mesh3D::SetSubMesh -> subMeshes is NULL.");

	if(index < subMeshCount)
	{
		subMeshes[index] = mesh;
		mesh->SetContainerMesh(this);
		mesh->Update();
	}
	else
	{
		Debug::PrintError("Mesh3D::SetSubMesh -> Index out of range.");
		return;
	}
}

SubMesh3D * Mesh3D::GetSubMesh(unsigned int index)
{
	NULL_CHECK(subMeshes,"Mesh3D::GetSubMesh -> subMeshes is NULL.", NULL);

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
