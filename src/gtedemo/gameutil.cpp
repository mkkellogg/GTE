#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "engine.h"
#include "game.h"
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
#include "scene/sceneobject.h"
#include "gameutil.h"

#include <iostream>
#include <memory>
#include <functional>

/*
 * Recursively search the scene hierarchy starting at [ref] for an
 * instance of SceneObject that contains a SkinnedMesh3DRenderer component,
 * but return just the SkinnedMesh3DRenderer instance.
 */
GTE::SkinnedMesh3DRendererSharedPtr GameUtil::FindFirstSkinnedMeshRenderer(GTE::SceneObjectSharedPtr ref)
{
	if(!ref.IsValid())return GTE::SkinnedMesh3DRendererSharedPtr::Null();

	if(ref->GetRenderer().IsValid())
	{
		GTE::SkinnedMesh3DRendererSharedPtr skinnedMeshRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::SkinnedMesh3DRenderer>(ref->GetRenderer());
		return skinnedMeshRenderer;
	}

	for(GTE::UInt32 i = 0; i < ref->GetChildrenCount(); i++)
	{
		GTE::SceneObjectSharedPtr childRef = ref->GetChildAt(i);
		GTE::SkinnedMesh3DRendererSharedPtr subRef = FindFirstSkinnedMeshRenderer(childRef);
		if(subRef.IsValid())return subRef;
	}

	return GTE::SkinnedMesh3DRendererSharedPtr::Null();
}


/*
 * Recursively search the scene hierarchy starting at [ref] for an
 * instance of SceneObject that contains a Mesh3D component, and return
 * the containing SceneObject instance.
 */
GTE::SceneObjectSharedPtr GameUtil::FindFirstSceneObjectWithMesh(GTE::SceneObjectSharedPtr ref)
{
	if(!ref.IsValid())return GTE::SceneObjectSharedPtr::Null();

	if(ref->GetMesh3D().IsValid())return ref;

	for(GTE::UInt32 i = 0; i < ref->GetChildrenCount(); i++)
	{
		GTE::SceneObjectSharedPtr childRef = ref->GetChildAt(i);
		GTE::SceneObjectSharedPtr subRef = FindFirstSceneObjectWithMesh(childRef);
		if(subRef.IsValid())return subRef;
	}

	return GTE::SceneObjectSharedPtr::Null();
}

/*
 * Recursively visit objects in the scene hierarchy with root at [ref] and for each
 * invoke [func] with the current SceneObject instance as the only parameter.
 */
void GameUtil::ProcessSceneObjects(GTE::SceneObjectSharedPtr ref, std::function<void(GTE::SceneObjectSharedPtr)> func)
{
	if(!ref.IsValid())return;

	// invoke [func]
	func(ref);

	for(GTE::UInt32 i = 0; i < ref->GetChildrenCount(); i++)
	{
		GTE::SceneObjectSharedPtr childRef = ref->GetChildAt(i);
		ProcessSceneObjects(childRef, func);
	}
}

/*
 * Set the SceneObject [root] and all children to be static.
 */
void GameUtil::SetAllObjectsStatic(GTE::SceneObjectSharedPtr root)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectSharedPtr current)
	{
		current->SetStatic(true);
	});
}

/*
 * Set the SceneObject [root] and all children to have layer mask [mask].
 */
void GameUtil::SetAllObjectsLayerMask(GTE::SceneObjectSharedPtr root, GTE::IntMask mask)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectSharedPtr current)
	{
		current->SetLayerMask(mask);
	});
}

/*
 * Set any mesh encountered in the scene hierarchy beginning with [root]
 * to use the standard shadow volume generation algorithm.
 */
void GameUtil::SetAllMeshesStandardShadowVolume(GTE::SceneObjectSharedPtr root)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectSharedPtr current)
	{
		GTE::Mesh3DFilterSharedPtr filter = current->GetMesh3DFilter();
		if(filter.IsValid())
		{
			filter->SetUseBackSetShadowVolume(false);
		}
	});
}

/*
* Set any mesh encountered in the scene hierarchy beginning with [root]
* to use a shadow volume offset specified by [offset].
*/
void GameUtil::SetAllMeshesShadowVolumeOffset(GTE::SceneObjectSharedPtr root, GTE::Real offset)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectSharedPtr current)
	{
		GTE::Mesh3DFilterSharedPtr filter = current->GetMesh3DFilter();
		if(filter.IsValid())
		{
			filter->SetUseCustomShadowVolumeOffset(true);
			filter->SetCustomShadowVolumeOffset(offset);
		}
	});
}

/*
 * Set any mesh encountered in the scene hierarchy beginning with [root]
 * to set shadow casting to [castShadows].
 */
void GameUtil::SetAllObjectsCastShadows(GTE::SceneObjectSharedPtr root, GTE::Bool castShadows)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectSharedPtr current)
	{
		GTE::Mesh3DFilterSharedPtr filter = current->GetMesh3DFilter();
		if(filter.IsValid())
		{
			filter->SetCastShadows(castShadows);
		}
	});
}

/*
* Set any mesh encountered in the scene hierarchy beginning with [root]
* to set shadow receiving to [castShadows].
*/
void GameUtil::SetAllObjectsReceiveShadows(GTE::SceneObjectSharedPtr root, GTE::Bool receiveShadows)
{
	ProcessSceneObjects(root, [=](GTE::SceneObjectSharedPtr current)
	{
		GTE::Mesh3DFilterSharedPtr filter = current->GetMesh3DFilter();
		if(filter.IsValid())
		{
			filter->SetReceiveShadows(receiveShadows);
		}
	});
}


GTE::SceneObjectSharedPtr GameUtil::AddMeshToScene(GTE::Mesh3DSharedPtr mesh, GTE::MaterialSharedPtr material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
	GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows)
{
	return AddMeshToScene(mesh, material, sx, sy, sz, rx, ry, rz, ra, tx, ty, tz, isStatic, castShadows, receiveShadows, true);
}

/*
 * Given an instance of Mesh3D [mesh] and and instance of Material [material], create an instance of SceneObject that
 * contains [mesh] and renders it using [material]. Scale the scene object's transform by [sx], [sy], [sz] in world
 * space, then rotate by [ra] degrees around the world space axis [rx], [ry], [rz], and then translate in world space by
 * [tx], [ty], tz]. If [isStatic] == true, then set the root SceneObject instance and all children to be static.
 *
 * This method is used to handle all the details of placing an arbitrary mesh somewhere in the scene at a specified orientation.
 */
GTE::SceneObjectSharedPtr GameUtil::AddMeshToScene(GTE::Mesh3DSharedPtr mesh, GTE::MaterialSharedPtr material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
	GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows, GTE::Bool useBackSetShadowVolume)
{
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();
	GTE::SceneObjectSharedPtr meshSceneObject = objectManager->CreateSceneObject();
	meshSceneObject->SetActive(true);
	GTE::Mesh3DFilterSharedPtr meshFilter = objectManager->CreateMesh3DFilter();
	meshFilter->SetUseBackSetShadowVolume(useBackSetShadowVolume);
	meshFilter->SetMesh3D(mesh);
	meshFilter->SetCastShadows(castShadows);
	meshFilter->SetReceiveShadows(receiveShadows);
	meshSceneObject->SetMesh3DFilter(meshFilter);
	GTE::Mesh3DRendererSharedPtr renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	meshSceneObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::Mesh3DRenderer, GTE::Renderer>(renderer));

	meshSceneObject->GetTransform().Scale(sx, sy, sz, false);
	if(ra != 0)meshSceneObject->GetTransform().Rotate(rx, ry, rz, ra, false);
	meshSceneObject->GetTransform().Translate(tx, ty, tz, false);

	if(isStatic)SetAllObjectsStatic(meshSceneObject);

	return meshSceneObject;
}
