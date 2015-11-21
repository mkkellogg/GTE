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

#include "engine.h"
#include "geometry/vector/vector3.h"
#include "asset/assetimporter.h"
#include "base/intmask.h"


class Scene
{
	protected:

	GTE::Vector3 baseCameraForward;

	public:

	Scene();
	virtual ~Scene();

	virtual GTE::SceneObjectSharedPtr GetSceneRoot() = 0;
	virtual void OnActivate() = 0;
	virtual void Update() = 0;
	virtual void Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject,
					   GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject) = 0;
};

#endif
