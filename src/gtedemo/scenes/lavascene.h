#ifndef _GTE_LAVASCENE_H_
#define _GTE_LAVASCENE_H_

#include "gtedemo/scene.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "asset/assetimporter.h"
#include "geometry/transform.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

//forward declarations
class LavaField;

class LavaScene : public Scene
{
	// the SceneObject instance at the root of the scene
	GTE::SceneObjectSharedPtr sceneRoot;

	// layer name for lava pool wall
	static const std::string LavaWallLayer;
	// layer name for lava pool island objects
	static const std::string LavaIslandObjectsLayer;
	// layer mask for lava wall layer
	GTE::IntMask lavaPlanarLayerMask;
	// layer mask for lava island layer objects
	GTE::IntMask lavaIslandObjectsLayerMask;
	// layer mask for player
	GTE::IntMask playerObjectLayerMask;

	// SceneObject that contains the spinning point light in the scene
	GTE::SceneObjectSharedPtr spinningPointLightObject;
	// scene lava
	LavaField * lavaField;
	// material for geometry near lavafield
	GTE::MaterialSharedPtr planarLitMaterial;
	// container lava lights
	std::vector<GTE::SceneObjectSharedPtr> lavaLightObjects;
	// The single cube in the scene
	GTE::SceneObjectSharedPtr cubeSceneObject;
	// global directional light
	GTE::SceneObjectSharedPtr directionalLightObject;

public:

	LavaScene();
	~LavaScene();

	GTE::SceneObjectSharedPtr GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject, GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject);
	void SetPlayerObjectLayerMask(GTE::IntMask playerObjectLayerMask);

	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject);

	GTE::SceneObjectSharedPtr GetSpinningPointLightObject();
	std::vector<GTE::SceneObjectSharedPtr>& GetLavaLightObjects();
	LavaField * GetLavaField();
};

#endif
