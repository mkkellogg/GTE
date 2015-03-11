#ifndef _GTE_POOLSCENE_H_
#define _GTE_POOLSCENE_H_

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

class PoolScene : public Scene
{
	// the SceneObject instance at the root of the scene
	SceneObjectRef sceneRoot;
	// point lights in this scene
	std::vector<SceneObjectRef> pointLights;
	// global directional light
	SceneObjectRef directionalLightObject;
	// water surface object
	SceneObjectRef waterObject;
	// camera for rending water
	CameraRef waterCamera;
	// material for rendering water
	MaterialRef waterMaterial;
	// main camera
	CameraRef mainCamera;

	public:

	PoolScene();
	~PoolScene();

	SceneObjectRef GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directionalLightObject, SceneObjectRef playerObject);
	void SetMainCamera(CameraRef camera);

	void SetupTerrain(AssetImporter& importer);
	void SetupStructures(AssetImporter& importer);
	void SetupPlants(AssetImporter& importer);
	void SetupExtra(AssetImporter& importer);
	void SetupLights(AssetImporter& importer, SceneObjectRef playerObject);

	std::vector<SceneObjectRef>& GetPointLights();
};

#endif
