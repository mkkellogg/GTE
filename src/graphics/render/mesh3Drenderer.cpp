#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/skinnedmesh3Dattrtransformer.h"
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
	EngineObjectManager *objectManager = Engine::Instance()->GetEngineObjectManager();
	if(index < subRenderers.size())
	{
		SubMesh3DRendererRef renderer = subRenderers[index];
		if(renderer.IsValid())
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

MaterialRef Mesh3DRenderer::GetMaterial(unsigned int index)
{
	if(index > GetMaterialCount())
	{
		Debug::PrintError("Mesh3DRenderer::SetMaterial -> Index is out of range.");
		return MaterialRef::Null();
	}

	return materials[index];
}

void Mesh3DRenderer::SetMaterial(unsigned int index, MaterialRef material)
{
	ASSERT_RTRN(material.IsValid(), "Mesh3DRenderer::SetMaterial -> material is NULL.");

	if(index > GetMaterialCount())
	{
		Debug::PrintError("Mesh3DRenderer::SetMaterial -> Index is out of range.");
		return;
	}

	materials[index] = material;
}

void Mesh3DRenderer::AddMaterial(MaterialRef material)
{
	ASSERT_RTRN(material.IsValid(), "Mesh3DRenderer::AddMaterial -> material is NULL.");
	materials.push_back(material);
}

void Mesh3DRenderer::UpdateFromMesh()
{
	ASSERT_RTRN(sceneObject.IsValid(),"Mesh3DRenderer::UpdateFromMeshes -> sceneObject is NULL.");

	Mesh3DRef mesh = sceneObject->GetMesh3D();
	ASSERT_RTRN(mesh.IsValid(),"Mesh3DRenderer::UpdateFromMeshes -> mesh is NULL.");

	UpdateFromMesh(mesh);
}

void Mesh3DRenderer::UpdateFromMesh(Mesh3DRef mesh)
{
	EngineObjectManager * engineObjectManager = Engine::Instance()->GetEngineObjectManager();
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
			SubMesh3DRendererRef renderer = engineObjectManager->CreateSubMesh3DRenderer();
			ASSERT_RTRN(renderer.IsValid(),"Mesh3DRenderer::UpdateFromMeshes(Mesh3DRef) -> could not create new SubMesh3DRenderer.");

			renderer->SetSubIndex(i);
			renderer->SetContainerRenderer(this);
			subRenderers.push_back(renderer);
		}
	}

	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		UpdateFromSubMesh(i);
	}
}

void Mesh3DRenderer::UpdateFromSubMesh(unsigned int index)
{
	if(index >= subRenderers.size())
	{
		Debug::PrintError("Mesh3DRenderer::UpdateFromMesh -> Index is out of range.");
		return;
	}

	SubMesh3DRendererRef renderer = subRenderers[index];
	renderer->UpdateFromMesh();
}

Mesh3DRef Mesh3DRenderer::GetMesh()
{
	ASSERT(sceneObject.IsValid(),"Mesh3DRenderer::GetMesh -> sceneObject is NULL.", Mesh3DRef::Null());

	Mesh3DRef mesh = sceneObject->GetMesh3D();
	ASSERT(mesh.IsValid(),"Mesh3DRenderer::GetMesh -> mesh is NULL.", Mesh3DRef::Null());

	return mesh;
}

SubMesh3DRef Mesh3DRenderer::GetSubMeshForSubRenderer(SubMesh3DRendererRef subRenderer)
{
	ASSERT(subRenderer.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> subRenderer is NULL.", SubMesh3DRef::Null());
	ASSERT(sceneObject.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> sceneObject is NULL.", SubMesh3DRef::Null());

	for(unsigned int i=0; i < subRenderers.size(); i++)
	{
		if(subRenderers[i] == subRenderer)
		{
			Mesh3DRef mesh = sceneObject->GetMesh3D();
			ASSERT(mesh.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> mesh is NULL.", SubMesh3DRef::Null());

			SubMesh3DRef subMesh = mesh->GetSubMesh(i);
			ASSERT(subMesh.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> subMesh is NULL.", SubMesh3DRef::Null());

			return subMesh;
		}
	}

	return SubMesh3DRef::Null();
}

SubMesh3DRef Mesh3DRenderer::GetSubMesh(unsigned int index)
{
	ASSERT(sceneObject.IsValid(),"Mesh3DRenderer::GetSubMesh -> sceneObject is NULL.", SubMesh3DRef::Null());

	Mesh3DRef mesh = sceneObject->GetMesh3D();
	ASSERT(mesh.IsValid(),"Mesh3DRenderer::GetSubMesh -> mesh is NULL.", SubMesh3DRef::Null());

	SubMesh3DRef subMesh = mesh->GetSubMesh(index);
	ASSERT(subMesh.IsValid(),"Mesh3DRenderer::GetSubMesh -> subMesh is NULL.", SubMesh3DRef::Null());

	return subMesh;
}

SubMesh3DRendererRef Mesh3DRenderer::GetSubRenderer(unsigned int index)
{
	if(index >= subRenderers.size())
	{
		Debug::PrintError("Mesh3DRenderer::GetSubRenderer -> Index is out of range.");
		return SubMesh3DRendererRef::Null();
	}

	return subRenderers[index];
}

unsigned int Mesh3DRenderer::GetSubRendererCount()
{
	return subRenderers.size();
}
