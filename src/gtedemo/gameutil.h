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
		static void SetAllObjectsCastShadows(GTE::SceneObjectRef root, bool castShadows);

		static GTE::SceneObjectRef AddMeshToScene(GTE::Mesh3DRef mesh, GTE::MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra, float tx, float ty, float tz,
									  bool isStatic, bool castShadows, bool receiveShadows);
};

#endif
