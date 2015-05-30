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

using namespace GTE;

class GameUtil
{
	public:

	static void ProcessSceneObjects(SceneObjectRef ref, std::function<void(SceneObjectRef)> func);
	static SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(SceneObjectRef ref);
	static SceneObjectRef FindFirstSceneObjectWithMesh(SceneObjectRef ref);
	static void SetAllObjectsStatic(SceneObjectRef root);
	static void SetAllObjectsLayerMask(SceneObjectRef root, IntMask mask);
	static void SetAllMeshesStandardShadowVolume(SceneObjectRef root);
	static void SetAllObjectsCastShadows(SceneObjectRef root, bool castShadows);

	static SceneObjectRef AddMeshToScene(Mesh3DRef mesh, MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra, float tx, float ty, float tz,
									  bool isStatic, bool castShadows, bool receiveShadows);

};

#endif
