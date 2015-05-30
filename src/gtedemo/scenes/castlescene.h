#ifndef _GTE_CASTLESCENE_H_
#define _GTE_CASTLESCENE_H_

#include "gtedemo/scene.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "asset/assetimporter.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

using namespace GTE;

class CastleScene : public Scene
{
	// the SceneObject instance at the root of the scene
	SceneObjectRef sceneRoot;
	// point lights in this scene
	std::vector<SceneObjectRef> pointLights;
	// global directional light
	SceneObjectRef directionalLightObject;

	public:

	CastleScene();
	~CastleScene();

	SceneObjectRef GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directionalLightObject, SceneObjectRef playerObject);

	void SetupTerrain(AssetImporter& importer);
	void SetupStructures(AssetImporter& importer);
	void SetupPlants(AssetImporter& importer);
	void SetupExtra(AssetImporter& importer);
	void SetupLights(AssetImporter& importer, SceneObjectRef playerObject);

	std::vector<SceneObjectRef>& GetPointLights();
};

#endif
