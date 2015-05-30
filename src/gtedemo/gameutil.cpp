#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <iostream>
#include <memory>
#include <functional>
#include "game.h"
#include "engine.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/mesh3D.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/rendertarget.h"
#include "graphics/render/material.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/color/color4.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2.h"
#include "graphics/uv/uv2array.h"
#include "geometry/matrix4x4.h"
#include "base/basevector4.h"
#include "geometry/transform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "gameutil.h"

/*
 * Recursively search the scene hierarchy starting at [ref] for an
 * instance of SceneObject that contains a SkinnedMesh3DRenderer component,
 * but return just the SkinnedMesh3DRenderer instance.
 */
GTE::SkinnedMesh3DRendererRef GameUtil::FindFirstSkinnedMeshRenderer(GTE::SceneObjectRef ref)
{
	if (!ref.IsValid())return GTE::SkinnedMesh3DRendererRef::Null();

	if(ref->GetSkinnedMesh3DRenderer().IsValid())return ref->GetSkinnedMesh3DRenderer();

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		GTE::SceneObjectRef childRef = ref->GetChildAt(i);
		GTE::SkinnedMesh3DRendererRef subRef = FindFirstSkinnedMeshRenderer(childRef);
		if(subRef.IsValid())return subRef;
	}

	return GTE::SkinnedMesh3DRendererRef::Null();
}


/*
 * Recursively search the scene hierarchy starting at [ref] for an
 * instance of SceneObject that contains a Mesh3D component, and return
 * the containing SceneObject instance.
 */
GTE::SceneObjectRef GameUtil::FindFirstSceneObjectWithMesh(GTE::SceneObjectRef ref)
{
	if (!ref.IsValid())return GTE::SceneObjectRef::Null();

	if(ref->GetMesh3D().IsValid())return ref;

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		GTE::SceneObjectRef childRef = ref->GetChildAt(i);
		GTE::SceneObjectRef subRef = FindFirstSceneObjectWithMesh(childRef);
		if(subRef.IsValid())return subRef;
	}

	return GTE::SceneObjectRef::Null();
}

/*
 * Recursively visit objects in the scene hierarchy with root at [ref] and for each
 * invoke [func] with the current SceneObject instance as the only parameter.
 */
void GameUtil::ProcessSceneObjects(GTE::SceneObjectRef ref, std::function<void(GTE::SceneObjectRef)> func)
{
	if(!ref.IsValid())return;

	// invoke [func]
	func(ref);

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		GTE::SceneObjectRef childRef = ref->GetChildAt(i);
		ProcessSceneObjects(childRef, func);
	}
}

/*
 * Set the SceneObject [root] and all children to be static.
 */
void GameUtil::SetAllObjectsStatic(GTE::SceneObjectRef root)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectRef current)
	{
		current->SetStatic(true);
	});
}

/*
 * Set the SceneObject [root] and all children to have layer mask [mask].
 */
void GameUtil::SetAllObjectsLayerMask(GTE::SceneObjectRef root, GTE::IntMask mask)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectRef current)
	{
		current->SetLayerMask(mask);
	});
}

/*
 * Set any mesh encountered in the scene hierarchy beginning with [root]
 * to use the standard shadow volume generation algorithm.
 */
void GameUtil::SetAllMeshesStandardShadowVolume(GTE::SceneObjectRef root)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectRef current)
	{
		GTE::SkinnedMesh3DRendererRef skinnedRenderer = current->GetSkinnedMesh3DRenderer();
		if(skinnedRenderer.IsValid())
		{
			for(unsigned int i = 0; i < skinnedRenderer->GetSubRendererCount(); i++)
			{
				skinnedRenderer->GetSubRenderer(i)->SetUseBackSetShadowVolume(false);
			}
		}

		GTE::Mesh3DRendererRef renderer = current->GetMesh3DRenderer();
		if(renderer.IsValid())
		{
			for(unsigned int i = 0; i < renderer->GetSubRendererCount(); i++)
			{
				renderer->GetSubRenderer(i)->SetUseBackSetShadowVolume(false);
			}
		}
	});
}

/*
 * Set any mesh encountered in the scene hierarchy beginning with [root]
 * to set shadow casting to [castShadows].
 */
void GameUtil::SetAllObjectsCastShadows(GTE::SceneObjectRef root, bool castShadows)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectRef current)
	{
		GTE::Mesh3DFilterRef filter = current->GetMesh3DFilter();
		if(filter.IsValid())
		{
			filter->SetCastShadows(castShadows);
		}
	});
}

/*
 * Given an instance of Mesh3D [mesh] and and instance of Material [material], create an instance of SceneObject that
 * contains [mesh] and renders it using [material]. Scale the scene object's transform by [sx], [sy], [sz] in world
 * space, then rotate by [ra] degrees around the world space axis [rx], [ry], [rz], and then translate in world space by
 * [tx], [ty], tz]. If [isStatic] == true, then set the root SceneObject instance and all children to be static.
 *
 * This method is used to handle all the details of placing an arbitrary mesh somewhere in the scene at a specified orientation.
 */
GTE::SceneObjectRef GameUtil::AddMeshToScene(GTE::Mesh3DRef mesh, GTE::MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra,
									float tx, float ty, float tz, bool isStatic, bool castShadows, bool receiveShadows)
{
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();
	GTE::SceneObjectRef meshSceneObject = objectManager->CreateSceneObject();
	meshSceneObject->SetActive(true);
	GTE::Mesh3DFilterRef meshFilter = objectManager->CreateMesh3DFilter();
	meshFilter->SetMesh3D(mesh);
	meshFilter->SetCastShadows(castShadows);
	meshFilter->SetReceiveShadows(receiveShadows);
	meshSceneObject->SetMesh3DFilter(meshFilter);
	GTE::Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	meshSceneObject->SetMesh3DRenderer(renderer);

	meshSceneObject->GetTransform().Scale(sx,sy,sz, false);
	if(ra != 0)meshSceneObject->GetTransform().Rotate(rx,ry,rz,ra,false);
	meshSceneObject->GetTransform().Translate(tx,ty,tz,false);

	if(isStatic)SetAllObjectsStatic(meshSceneObject);

	return meshSceneObject;
}
