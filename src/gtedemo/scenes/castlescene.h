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
	GTE::SceneObjectSharedPtr sceneRoot;
	// point lights in this scene
	std::vector<GTE::SceneObjectSharedPtr> pointLights;
	// global directional light
	GTE::SceneObjectSharedPtr directionalLightObject;
	// moving light A
	GTE::SceneObjectSharedPtr movingLightA;

public:

	CastleScene();
	~CastleScene();

	GTE::SceneObjectSharedPtr GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject, GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject);

	void SetupParticleSystems(GTE::AssetImporter& importer);
	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupPlants(GTE::AssetImporter& importer);
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject);

	std::vector<GTE::SceneObjectSharedPtr>& GetPointLights();
};

#endif
