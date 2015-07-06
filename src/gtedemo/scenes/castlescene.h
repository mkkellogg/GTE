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

class CastleScene : public Scene
{
	// the SceneObject instance at the root of the scene
	GTE::SceneObjectRef sceneRoot;
	// point lights in this scene
	std::vector<GTE::SceneObjectRef> pointLights;
	// global directional light
	GTE::SceneObjectRef directionalLightObject;
	// moving light A
	GTE::SceneObjectRef movingLightA;

	public:

	CastleScene();
	~CastleScene();

	GTE::SceneObjectRef GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectRef ambientLightObject, GTE::SceneObjectRef directionalLightObject, GTE::SceneObjectRef playerObject);

	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupPlants(GTE::AssetImporter& importer);
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject);

	std::vector<GTE::SceneObjectRef>& GetPointLights();
};

#endif
