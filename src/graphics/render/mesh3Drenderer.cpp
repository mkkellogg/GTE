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
	for(unsigned int i =0; i< renderers.size(); i++)
	{
		DestroyRenderer(i);
	}
	renderers.clear();
}

void Mesh3DRenderer::DestroyRenderer(unsigned int index)
{
	if(index < renderers.size())
	{
		SubMesh3DRenderer * renderer = renderers[index];
		SAFE_DELETE(renderer);
		renderers.erase(renderers.begin() + index);
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
}

void Mesh3DRenderer::UpdateFromMeshes()
{
	NULL_CHECK_RTRN(sceneObject,"Mesh3DRenderer::UpdateFromMeshes -> sceneObject is NULL.");

	Mesh3D * mesh = sceneObject->GetMesh3D();
	NULL_CHECK_RTRN(mesh,"Mesh3DRenderer::UpdateFromMeshes -> mesh is NULL.");

	EngineObjectManager * engineObjectManager = EngineObjectManager::Instance();
	unsigned int subMeshCount =  mesh->GetSubMeshCount();

	if(subMeshCount < renderers.size())
	{
		for(unsigned int i = renderers.size(); i > subMeshCount; i--)
		{
			DestroyRenderer(i-1);
		}
	}
	else if(subMeshCount > renderers.size())
	{
		for(unsigned int i = subMeshCount; i <= renderers.size(); i++)
		{
			SubMesh3DRenderer * renderer = engineObjectManager->CreateSubMesh3DRenderer();
			NULL_CHECK_RTRN(mesh,"Mesh3DRenderer::UpdateFromMeshes -> could not create new SubMesh3DRenderer.");

			renderers.push_back(renderer);
		}
	}

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		UpdateFromMesh(i);
	}
}

void Mesh3DRenderer::UpdateFromMesh(unsigned int index)
{
	if(index > renderers.size())
	{
		Debug::PrintError("Mesh3DRenderer::UpdateFromMesh -> Index is out of range.");
		return;
	}

	SubMesh3DRenderer * renderer = renderers[index];
	renderer->UpdateFromMesh();
}
