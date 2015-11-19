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
	// temp variable used to help flicker the camp fire light intensity
	GTE::Real lastCampFireLightIntenistyAdjuster;
	// temp variable used to help flicker the camp fire light position
	GTE::Vector3 lastCampFireLightPositionAdjuster;
	// local offset of the campfire light relative to the campfire model
	GTE::Vector3 campFireLightLocalOffset;
	// camp fire light's scene object
	GTE::SceneObjectSharedPtr campFireLightObject;
	// camp fire's light
	GTE::LightSharedPtr campFireLight;
	// last time light flickering was updated
	GTE::Real lastFlickerTime;
	// scene obejct that contains the smoke particle system
	GTE::SceneObjectSharedPtr smokeSystemObject;

	static const std::string SmokeLayer;

	void FlickerCampFireLightLight();
	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupPlants(GTE::AssetImporter& importer);
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject);
	void SetupCampfire(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject);

public:

	CastleScene();
	~CastleScene();

	GTE::SceneObjectSharedPtr GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject, GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject);
	std::vector<GTE::SceneObjectSharedPtr>& GetPointLights();
	void ToggleSmoke();
};

#endif
