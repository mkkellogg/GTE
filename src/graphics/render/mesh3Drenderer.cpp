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
#include "debug/gtedebug.h"

/*
 * Default constructor
 */
Mesh3DRenderer::Mesh3DRenderer()
{

}

/*
 * Clean-up
 */
Mesh3DRenderer::~Mesh3DRenderer()
{

}

/*
 * Deallocate and destroy each sub-renderer contained by this instance.
 */
void Mesh3DRenderer::DestroyRenderers()
{
	for(unsigned int i =0; i< subRenderers.size(); i++)
	{
		DestroyRenderer(i);
	}
	subRenderers.clear();
}

/*
 * Deallocate and destroy the sub-renderer at [index] in the member list of
 * sub-renderers: [subRenderers].
 */
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

/*
 * Get the number of materials attached to this instance.
 */
unsigned int Mesh3DRenderer::GetMaterialCount() const
{
	return materials.size();
}

/*
 * Get a reference to the material at [index] in the member list of materials, [materials].
 */
MaterialRef Mesh3DRenderer::GetMaterial(unsigned int index)
{
	ASSERT(index < GetMaterialCount(), "Mesh3DRenderer::GetMaterial -> Index is out of range.", MaterialRef::Null());

	return materials[index];
}

/*
 * Set the material at [index] in the member list of materials, [materials].
 */
void Mesh3DRenderer::SetMaterial(unsigned int index, MaterialRef material)
{
	ASSERT_RTRN(material.IsValid(), "Mesh3DRenderer::SetMaterial -> material is NULL.");
	ASSERT_RTRN(index < GetMaterialCount(), "Mesh3DRenderer::SetMaterial -> Index is out of range.");

	materials[index] = material;
}

/*
 * Add a material to the member list of materials, [materials].
 */
void Mesh3DRenderer::AddMaterial(MaterialRef material)
{
	ASSERT_RTRN(material.IsValid(), "Mesh3DRenderer::AddMaterial -> material is NULL.");
	materials.push_back(material);
}

/*
 * This method should be triggered every time the mesh for which this renderer is responsible
 * gets set.
 *
 * GetTargetMesh() returns the mesh in question. Typically it will be the Mesh3D object that is
 * attached to the same SceneObject as this renderer. If the target mesh is valid, it calls UpdateFromMesh()
 * specifically for that mesh.
 */
void Mesh3DRenderer::InitializeForMesh()
{
	ASSERT_RTRN(sceneObject.IsValid(),"Mesh3DRenderer::UpdateFromMesh -> sceneObject is NULL.");

	Mesh3DRef mesh = GetTargetMesh();
	ASSERT_RTRN(mesh.IsValid(),"Mesh3DRenderer::UpdateFromMesh -> mesh is NULL.");

	InitializeForMesh(mesh);
}

/*
 * Update this renderer and prepare it for rendering of the sub-meshes contained in [mesh].
 */
void Mesh3DRenderer::InitializeForMesh(Mesh3DRef mesh)
{
	EngineObjectManager * engineObjectManager = Engine::Instance()->GetEngineObjectManager();
	unsigned int subMeshCount =  mesh->GetSubMeshCount();

	// if the number of sub-renderers currently contained by this instance is larger than
	// the number of sub-meshes contained by [mesh], then delete the excess sub-renderers
	if(subMeshCount < subRenderers.size())
	{
		for(unsigned int i = subRenderers.size(); i > subMeshCount; i--)
		{
			DestroyRenderer(i-1);
		}
	}
	// if the number of sub-renderers currently contained by this instance is less than
	// the number of sub-meshes contained by [mesh], create enough new sub-renderers to make those
	// numbers match
	else if(subMeshCount > subRenderers.size())
	{
		for(unsigned int i = subRenderers.size(); i < subMeshCount; i++)
		{
			SubMesh3DRendererRef renderer = engineObjectManager->CreateSubMesh3DRenderer();
			ASSERT_RTRN(renderer.IsValid(),"Mesh3DRenderer::UpdateFromMesh(Mesh3DRef) -> could not create new SubMesh3DRenderer.");

			renderer->SetTargetSubMeshIndex(i);
			renderer->SetContainerRenderer(this);
			subRenderers.push_back(renderer);
		}
	}

	// for each sub-renderer, call UpdateFromSubMesh() to update the sub-renderer
	// for its corresponding sub-mesh in [mesh]
	for(unsigned int i = 0; i < subMeshCount; i++)
	{
		UpdateFromSubMesh(i);
	}
}

/*
 * Update the sub-render at [index] in the member list of sub-renderers [subRenderers]
 * for the sub-mesh at [index] in the target mesh for this renderer.
 */
void Mesh3DRenderer::UpdateFromSubMesh(unsigned int index)
{
	ASSERT_RTRN(index <  subRenderers.size(), "Mesh3DRenderer::UpdateFromSubMesh -> Index is out of range.");

	SubMesh3DRendererRef renderer = subRenderers[index];
	renderer->UpdateFromMesh();
}

/*
 * Get the target mesh for this renderer. The Mesh3D object that is attached to the same SceneObject
 * as this renderer will implicitly be the target mesh.
 */
Mesh3DRef Mesh3DRenderer::GetTargetMesh()
{
	ASSERT(sceneObject.IsValid(),"Mesh3DRenderer::GetTargetMesh -> sceneObject is NULL.", Mesh3DRef::Null());

	Mesh3DRef mesh = sceneObject->GetMesh3D();

	return mesh;
}

/*
 * Get the sub-mesh contained in the target mesh for this renderer that corresponds to [subRenderer]. This will be
 * the SubMesh3D instance that [subRenderer] is responsible for rendering.
 */
SubMesh3DRef Mesh3DRenderer::GetSubMeshForSubRenderer(SubMesh3DRendererRef subRenderer)
{
	ASSERT(subRenderer.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> subRenderer is NULL.", SubMesh3DRef::Null());
	ASSERT(sceneObject.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> sceneObject is NULL.", SubMesh3DRef::Null());

	// this loop finds the index in [subRenderers] to which [subRenderer] belongs
	for(unsigned int i=0; i < subRenderers.size(); i++)
	{
		if(subRenderers[i] == subRenderer)
		{
			Mesh3DRef mesh = GetTargetMesh();
			ASSERT(mesh.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> mesh is NULL.", SubMesh3DRef::Null());

			SubMesh3DRef subMesh = mesh->GetSubMesh(i);
			ASSERT(subMesh.IsValid(),"Mesh3DRenderer::GetSubMeshForSubRenderer -> subMesh is NULL.", SubMesh3DRef::Null());

			return subMesh;
		}
	}

	return SubMesh3DRef::Null();
}

/*
 * Get the sub-mesh at [index] in the target mesh for this renderer.
 */
SubMesh3DRef Mesh3DRenderer::GetSubMesh(unsigned int index)
{
	ASSERT(sceneObject.IsValid(),"Mesh3DRenderer::GetSubMesh -> sceneObject is NULL.", SubMesh3DRef::Null());

	Mesh3DRef mesh = GetTargetMesh();
	ASSERT(mesh.IsValid(),"Mesh3DRenderer::GetSubMesh -> mesh is NULL.", SubMesh3DRef::Null());

	SubMesh3DRef subMesh = mesh->GetSubMesh(index);
	ASSERT(subMesh.IsValid(),"Mesh3DRenderer::GetSubMesh -> subMesh is NULL.", SubMesh3DRef::Null());

	return subMesh;
}

/*
 * Get the sub-renderer at [index] in the member list of sub-renderers, [subRenderers].
 */
SubMesh3DRendererRef Mesh3DRenderer::GetSubRenderer(unsigned int index)
{
	if(index >= subRenderers.size())
	{
		Debug::PrintError("Mesh3DRenderer::GetSubRenderer -> Index is out of range.");
		return SubMesh3DRendererRef::Null();
	}

	return subRenderers[index];
}

/*
 * Get the number of sub-renderers managed by this renderer.
 */
unsigned int Mesh3DRenderer::GetSubRendererCount() const
{
	return subRenderers.size();
}
