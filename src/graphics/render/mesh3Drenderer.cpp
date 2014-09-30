#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "object/engineobjectmanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/skinnedmeshattrtransformer.h"
#include "global/global.h"
#include "ui/debug.h"

Mesh3DRenderer::Mesh3DRenderer()
{

}

Mesh3DRenderer::~Mesh3DRenderer()
{

}

void Mesh3DRenderer::DestroyRenderers()
{
	for(unsigned int i =0; i< subRenderers.size(); i++)
	{
		DestroyRenderer(i);
	}
	subRenderers.clear();
}

void Mesh3DRenderer::DestroyRenderer(unsigned int index)
{
	EngineObjectManager *objectManager = EngineObjectManager::Instance();
	if(index < subRenderers.size())
	{
		SubMesh3DRenderer * renderer = subRenderers[index];
		if(renderer != NULL)
		{
			objectManager->DestroySubMesh3DRenderer(renderer);
		}
		subRenderers.erase(subRenderers.begin() + index);
	}
}

unsigned int Mesh3DRenderer::GetMaterialCount()
{
	return materials.size();
}

Material * Mesh3DRenderer::GetMaterial(unsigned int index)
{
	if(index > GetMaterialCount())
	{
		Debug::PrintError("Mesh3DRenderer::SetMaterial -> Index is out of range.");
		return NULL;
	}

	return materials[index];
}

void Mesh3DRenderer::SetMaterial(unsigned int index, Material * material)
{
	NULL_CHECK_RTRN(material, "Mesh3DRenderer::SetMaterial -> material is NULL.");

	if(index > GetMaterialCount())
	{
		Debug::PrintError("Mesh3DRenderer::SetMaterial -> Index is out of range.");
		return;
	}

	materials[index] = material;
}

void Mesh3DRenderer::AddMaterial(Material * material)
{
	NULL_CHECK_RTRN(material, "Mesh3DRenderer::AddMaterial -> material is NULL.");
	materials.push_back(material);
}

void Mesh3DRenderer::UpdateFromMeshes()
{
	NULL_CHECK_RTRN(sceneObject,"Mesh3DRenderer::UpdateFromMeshes -> sceneObject is NULL.");

	Mesh3D * mesh = sceneObject->GetMesh3D();
	NULL_CHECK_RTRN(mesh,"Mesh3DRenderer::UpdateFromMeshes -> mesh is NULL.");

	EngineObjectManager * engineObjectManager = EngineObjectManager::Instance();
	unsigned int subMeshCount =  mesh->GetSubMeshCount();

	if(subMeshCount < subRenderers.size())
	{
		for(unsigned int i = subRenderers.size(); i > subMeshCount; i--)
		{
			DestroyRenderer(i-1);
		}
	}
	else if(subMeshCount > subRenderers.size())
	{
		for(unsigned int i = subRenderers.size(); i < subMeshCount; i++)
		{
			SubMesh3DRenderer * renderer = engineObjectManager->CreateSubMesh3DRenderer();
			NULL_CHECK_RTRN(mesh,"Mesh3DRenderer::UpdateFromMeshes -> could not create new SubMesh3DRenderer.");

			renderer->SetContainerRenderer(this);
			subRenderers.push_back(renderer);
		}
	}

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		UpdateFromMesh(i);
	}
}

void Mesh3DRenderer::UpdateFromMesh(unsigned int index)
{
	if(index >= subRenderers.size())
	{
		Debug::PrintError("Mesh3DRenderer::UpdateFromMesh -> Index is out of range.");
		return;
	}

	SubMesh3DRenderer * renderer = subRenderers[index];
	renderer->UpdateFromMesh();
}

Mesh3D * Mesh3DRenderer::GetMesh()
{
	NULL_CHECK(sceneObject,"Mesh3DRenderer::GetMesh -> sceneObject is NULL.", NULL);

	Mesh3D * mesh = sceneObject->GetMesh3D();
	NULL_CHECK(mesh,"Mesh3DRenderer::GetMesh -> mesh is NULL.", NULL);

	return mesh;
}

SubMesh3D * Mesh3DRenderer::GetSubMeshForSubRenderer(SubMesh3DRenderer * subRenderer)
{
	NULL_CHECK(subRenderer,"Mesh3DRenderer::GetSubMeshForSubRenderer -> subRenderer is NULL.", NULL);
	NULL_CHECK(sceneObject,"Mesh3DRenderer::GetSubMeshForSubRenderer -> sceneObject is NULL.", NULL);

	for(unsigned int i=0; i < subRenderers.size(); i++)
	{
		if(subRenderers[i] == subRenderer)
		{
			Mesh3D * mesh = sceneObject->GetMesh3D();
			NULL_CHECK(mesh,"Mesh3DRenderer::GetSubMeshForSubRenderer -> mesh is NULL.", NULL);

			SubMesh3D * subMesh = mesh->GetSubMesh(i);
			NULL_CHECK(subMesh,"Mesh3DRenderer::GetSubMeshForSubRenderer -> subMesh is NULL.", NULL);

			return subMesh;
		}
	}

	return NULL;
}

SubMesh3D * Mesh3DRenderer::GetSubMesh(unsigned int index)
{
	NULL_CHECK(sceneObject,"Mesh3DRenderer::GetSubMesh -> sceneObject is NULL.", NULL);

	Mesh3D * mesh = sceneObject->GetMesh3D();
	NULL_CHECK(mesh,"Mesh3DRenderer::GetSubMeshForSubRenderer -> mesh is NULL.", NULL);

	SubMesh3D * subMesh = mesh->GetSubMesh(index);
	NULL_CHECK(subMesh,"Mesh3DRenderer::GetSubMeshForSubRenderer -> subMesh is NULL.", NULL);

	return subMesh;
}

SubMesh3DRenderer * Mesh3DRenderer::GetSubRenderer(unsigned int index)
{
	if(index >= subRenderers.size())
	{
		Debug::PrintError("Mesh3DRenderer::GetSubRenderer -> Index is out of range.");
		return NULL;
	}

	return subRenderers[index];
}

unsigned int Mesh3DRenderer::GetSubRendererCount()
{
	return subRenderers.size();
}
