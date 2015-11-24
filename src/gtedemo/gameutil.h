/*
 * Class: Scene
 *
 * Author: Mark Kellogg
 *
 * Part of the GTE engine demo, GameUtil is a utility class
 * for performing common useful functions.
 */

#ifndef _GTE_GAMEUTIL_H_
#define _GTE_GAMEUTIL_H_

#include <functional>

#include "engine.h"
#include "graphics/stdattributes.h"
#include "base/intmask.h"

class GameUtil
{
public:

	static void ProcessSceneObjects(GTE::SceneObjectSharedPtr ref, std::function<void(GTE::SceneObjectSharedPtr)> func);
	static GTE::SkinnedMesh3DRendererSharedPtr FindFirstSkinnedMeshRenderer(GTE::SceneObjectSharedPtr ref);
	static GTE::SceneObjectSharedPtr FindFirstSceneObjectWithMesh(GTE::SceneObjectSharedPtr ref);
	static void SetAllObjectsStatic(GTE::SceneObjectSharedPtr root);
	static void SetAllObjectsLayerMask(GTE::SceneObjectSharedPtr root, GTE::IntMask mask);
	static void SetAllMeshesStandardShadowVolume(GTE::SceneObjectSharedPtr root);
	static void SetAllMeshesShadowVolumeOffset(GTE::SceneObjectSharedPtr root, GTE::Real offset);
	static void SetAllObjectsCastShadows(GTE::SceneObjectSharedPtr root, GTE::Bool castShadows);
	static void SetAllObjectsReceiveShadows(GTE::SceneObjectSharedPtr root, GTE::Bool receiveShadows);

	static GTE::SceneObjectSharedPtr AddMeshToScene(GTE::Mesh3DSharedPtr mesh, GTE::MaterialSharedPtr material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
											  GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows);

	static GTE::SceneObjectSharedPtr AddMeshToScene(GTE::Mesh3DSharedPtr mesh, GTE::MaterialSharedPtr material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
											  GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows, GTE::Bool useBackSetShadowVolume);

	static GTE::SceneObjectSharedPtr AddMeshToScene(GTE::Mesh3DSharedPtr mesh, GTE::MultiMaterialSharedPtr material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
													GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows);

	static GTE::SceneObjectSharedPtr AddMeshToScene(GTE::Mesh3DSharedPtr mesh, GTE::MultiMaterialSharedPtr material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
													GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows, GTE::Bool useBackSetShadowVolume);
};

#endif
