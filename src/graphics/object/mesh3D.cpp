#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <unordered_map>
#include <functional>
#include <vector>

#include "engine.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/engineobject.h"
#include "object/engineobjectmanager.h"
#include "mesh3D.h"
#include "submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "global/global.h"
#include "debug/debug.h"


Mesh3D::Mesh3D(unsigned int subMeshCount)
{
	if(subMeshCount<=0)subMeshCount=1;
	this->subMeshCount = subMeshCount;
	lightCullType = LightCullType::SphereOfInfluence;
	castShadows = false;
	receiveShadows = false;
}

Mesh3D::~Mesh3D()
{
	Destroy();
}

void Mesh3D::Destroy()
{
	subMeshes.clear();
}

unsigned int Mesh3D::GetSubMeshCount()
{
	return subMeshCount;
}

bool Mesh3D::Init()
{
	Destroy();

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		subMeshes.push_back(SubMesh3DRef::Null());
	}

	return true;
}

void Mesh3D::SendDataToRenderer(unsigned int index)
{
	if( index >= subMeshCount)
	{
		Debug::PrintError("Mesh3D::SendDataToRenderer -> Index out of range.");
		return;
	}

	ASSERT_RTRN(sceneObject.IsValid()," Mesh3D::SendDataToRenderer -> sceneObject is NULL.");

	SubMesh3DRef subMesh = subMeshes[index];
	ASSERT_RTRN(subMesh.IsValid()," Mesh3D::SendDataToRenderer -> subMesh is NULL.");

	ASSERT_RTRN(IsAttachedToSceneObject()," Mesh3D::SendDataToRenderer -> Mesh is not attached to a scene object.");

	Mesh3DRendererRef renderer = sceneObject->GetMesh3DRenderer();
	ASSERT_RTRN(renderer.IsValid()," Mesh3D::SendDataToRenderer -> renderer is NULL.");

	renderer->UpdateFromSubMesh(index);
}

void Mesh3D::CalculateSphereOfInfluence()
{
	Point3 average;
	unsigned int validSubMeshes = 0;
	for(unsigned int i = 0; i < subMeshCount; i++)
	{

		SubMesh3DRef subMesh = subMeshes[i];
		if(subMesh.IsValid())
		{
			const Point3 temp = subMesh->GetCenter();
			average.x += temp.x;
			average.y += temp.y;
			average.z += temp.z;
			validSubMeshes++;
		}
	}

	if(validSubMeshes > 0)
	{
		average.x /= (float)validSubMeshes;
		average.y /= (float)validSubMeshes;
		average.z /= (float)validSubMeshes;
	}

	center = average;

	float maxSoiX = 0;
	float maxSoiY = 0;
	float maxSoiZ = 0;
	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		SubMesh3DRef subMesh = subMeshes[i];
		if(subMesh.IsValid())
		{
			const Point3 temp = subMesh->GetCenter();

			float offsetX = GTEMath::Abs(center.x - temp.x);
			float offsetY = GTEMath::Abs(center.y - temp.y);
			float offsetZ = GTEMath::Abs(center.z - temp.z);

			float soiX = offsetX + GTEMath::Abs(subMesh->GetSphereOfInfluenceX().x);
			float soiY = offsetY + GTEMath::Abs(subMesh->GetSphereOfInfluenceX().y);
			float soiZ = offsetZ + GTEMath::Abs(subMesh->GetSphereOfInfluenceX().z);

			if(soiX > maxSoiX)maxSoiX = soiX;
			if(soiY > maxSoiY)maxSoiY = soiY;
			if(soiZ > maxSoiZ)maxSoiZ = soiZ;
		}
	}

	sphereOfInfluenceX.Set(maxSoiX,0,0);
	sphereOfInfluenceY.Set(0,maxSoiY,0);
	sphereOfInfluenceZ.Set(0,0,maxSoiZ);
}

void Mesh3D::Update()
{
	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		if(subMeshes[i].IsValid())
		{
			subMeshes[i]->Update();
		}
	}
}

void Mesh3D::SetSubMesh(SubMesh3DRef mesh, unsigned int index)
{
	ASSERT_RTRN(mesh.IsValid(),"Mesh3D::SetSubMesh -> mesh is NULL.");

	if(index < subMeshCount)
	{
		subMeshes[index] = mesh;
		mesh->SetContainerMesh(this);
		mesh->SetSubIndex(index);
		mesh->Update();
	}
	else
	{
		Debug::PrintError("Mesh3D::SetSubMesh -> Index out of range.");
		return;
	}
}

SubMesh3DRef Mesh3D::GetSubMesh(unsigned int index)
{
	if(index < subMeshCount)
	{
		return subMeshes[index];
	}
	else
	{
		Debug::PrintError("Mesh3D::GetSubMesh -> Index out of range.");
		return SubMesh3DRef::Null();
	}
}

bool Mesh3D::IsAttachedToSceneObject()
{
	return sceneObject.IsValid();
}

bool Mesh3D::SceneObjectHasRenderer()
{
	if(!IsAttachedToSceneObject())return false;
	if(!sceneObject->GetMesh3DRenderer().IsValid())return false;
	return true;
}

const Point3& Mesh3D::GetCenter() const
{
	return center;
}

const Vector3& Mesh3D::GetSphereOfInfluenceX() const
{
	return sphereOfInfluenceX;
}

const Vector3& Mesh3D::GetSphereOfInfluenceY() const
{
	return sphereOfInfluenceY;
}

const Vector3& Mesh3D::GetSphereOfInfluenceZ() const
{
	return sphereOfInfluenceZ;
}

LightCullType Mesh3D::GetLightCullType() const
{
	return lightCullType;
}

void Mesh3D::SetCastShadows(bool castShadows)
{
	this->castShadows = castShadows;
}

bool Mesh3D::GetCastShadows()
{
	return castShadows;
}

void Mesh3D::SetReceiveShadows(bool receiveShadows)
{
	this->receiveShadows = receiveShadows;
}

bool Mesh3D::GetReceiveShadows()
{
	return receiveShadows;
}
