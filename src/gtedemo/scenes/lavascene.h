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
	GTE::SceneObjectRef sceneRoot;

	// layer name for lava pool wall
	static const std::string LavaWallLayer;
	// layer name for lava pool island
	static const std::string LavaIslandLayer;
	// layer name for lava pool island objects
	static const std::string LavaIslandObjectsLayer;
	// layer mask for lava wall layer
	GTE::IntMask lavaWallLayerMask;
	// layer mask for lava island layer
	GTE::IntMask lavaIslandLayerMask;
	// layer mask for lava island layer objects
	GTE::IntMask lavaIslandObjectsLayerMask;

	// SceneObject that contains the spinning point light in the scene
	GTE::SceneObjectRef spinningPointLightObject;
	// scene lava
	LavaField * lavaField;
	// container lava lights
	std::vector<GTE::SceneObjectRef> lavaLightObjects;
	// The single cube in the scene
	GTE::SceneObjectRef cubeSceneObject;
	// global directional light
	GTE::SceneObjectRef directionalLightObject;

	public:

	LavaScene();
	~LavaScene();

	GTE::SceneObjectRef GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectRef ambientLightObject, GTE::SceneObjectRef directionalLightObject, GTE::SceneObjectRef playerObject);

	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject);

	GTE::SceneObjectRef GetSpinningPointLightObject();
	std::vector<GTE::SceneObjectRef>& GetLavaLightObjects();
	LavaField * GetLavaField();
};

#endif
