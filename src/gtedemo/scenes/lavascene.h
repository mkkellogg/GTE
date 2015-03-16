#ifndef _GTE_LAVASCENE_H_
#define _GTE_LAVASCENE_H_

//forward declarations
class AssetImporter;
class Vector3;
class Quaternion;
class LavaField;

#include "gtedemo/scene.h"
#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

class LavaScene : public Scene
{
	// the SceneObject instance at the root of the scene
	SceneObjectRef sceneRoot;

	// layer name for lava pool wall
	static const std::string LavaWallLayer;
	// layer name for lava pool island
	static const std::string LavaIslandLayer;
	// layer name for lava pool island objects
	static const std::string LavaIslandObjectsLayer;
	// layer mask for lava wall layer
	IntMask lavaWallLayerMask;
	// layer mask for lava island layer
	IntMask lavaIslandLayerMask;
	// layer mask for lava island layer objects
	IntMask lavaIslandObjectsLayerMask;

	// SceneObject that contains the spinning point light in the scene
	SceneObjectRef spinningPointLightObject;
	// scene lava
	LavaField * lavaField;
	// container lava lights
	std::vector<SceneObjectRef> lavaLightObjects;
	// The single cube in the scene
	SceneObjectRef cubeSceneObject;
	// global directional light
	SceneObjectRef directionalLightObject;

	public:

	LavaScene();
	~LavaScene();

	SceneObjectRef GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directionalLightObject, SceneObjectRef playerObject);

	void SetupTerrain(AssetImporter& importer);
	void SetupStructures(AssetImporter& importer);
	void SetupExtra(AssetImporter& importer);
	void SetupLights(AssetImporter& importer, SceneObjectRef playerObject);

	SceneObjectRef GetSpinningPointLightObject();
	std::vector<SceneObjectRef>& GetLavaLightObjects();
	LavaField * GetLavaField();
};

#endif
