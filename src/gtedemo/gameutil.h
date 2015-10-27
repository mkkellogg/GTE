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
#include "object/enginetypes.h"
#include "graphics/stdattributes.h"
#include "base/intmask.h"

class GameUtil
{
public:

	static void ProcessSceneObjects(GTE::SceneObjectRef ref, std::function<void(GTE::SceneObjectRef)> func);
	static GTE::SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(GTE::SceneObjectRef ref);
	static GTE::SceneObjectRef FindFirstSceneObjectWithMesh(GTE::SceneObjectRef ref);
	static void SetAllObjectsStatic(GTE::SceneObjectRef root);
	static void SetAllObjectsLayerMask(GTE::SceneObjectRef root, GTE::IntMask mask);
	static void SetAllMeshesStandardShadowVolume(GTE::SceneObjectRef root);
	static void SetAllObjectsCastShadows(GTE::SceneObjectRef root, GTE::Bool castShadows);

	static GTE::SceneObjectRef AddMeshToScene(GTE::Mesh3DRef mesh, GTE::MaterialRef material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
											  GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows);

	static GTE::SceneObjectRef AddMeshToScene(GTE::Mesh3DRef mesh, GTE::MaterialRef material, GTE::Real sx, GTE::Real sy, GTE::Real sz, GTE::Real rx, GTE::Real ry, GTE::Real rz, GTE::Real ra, GTE::Real tx, GTE::Real ty, GTE::Real tz,
											  GTE::Bool isStatic, GTE::Bool castShadows, GTE::Bool receiveShadows, GTE::Bool useBackSetShadowVolume);
};

#endif
