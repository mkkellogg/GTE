/*
 * Class: Scene
 *
 * Author: Mark Kellogg
 *
 * Part of the GTE engine demo, Scene is a base class
 * for grouping scene objects and components.
 */

#ifndef _GTE_SCENE_H_
#define _GTE_SCENE_H_

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "asset/assetimporter.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

using namespace GTE;

class Scene
{
	protected:

	Vector3 baseCameraForward;

	public:

	Scene();
	virtual ~Scene();

	virtual SceneObjectRef GetSceneRoot() = 0;
	virtual void OnActivate() = 0;
	virtual void Update() = 0;
	virtual void Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directionalLightObject, SceneObjectRef playerObject) = 0;
};

#endif
