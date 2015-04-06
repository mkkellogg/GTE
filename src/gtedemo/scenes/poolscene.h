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
	// player object
	SceneObjectRef playerObject;
	// position of player in previous frame
	Point3 lastPlayerPos;
	// main camera
	CameraRef mainCamera;

	// water surface object
	SceneObjectRef waterSurfaceSceneObject;
	// camera for rendering water reflection
	CameraRef waterReflectionCamera;
	// camera for rendering water's surface
	CameraRef waterSurfaceCamera;
	// material for rendering water
	MaterialRef waterMaterial;
	// height maps for water surface
	RenderTargetRef waterHeights[2];
	// normal maps for water surface
	RenderTargetRef waterNormals;
	// material for rendering the height maps
	MaterialRef waterHeightsMaterial;
	// material for rendering the normal map
	MaterialRef waterNormalsMaterial;
	// material for adding a new water drop
	MaterialRef waterDropMaterial;

	// last time drop was added to pool
	float lastWaterDropTime;
	// last time water simulation advanced
	float lastWaterSimAdvanceTime;
	// current height map index
	unsigned int currentHeightMapIndex;

	// water mesh resolution
	const unsigned int waterMeshResolution = 128;
	// water height map resolution
	const unsigned int waterHeightMapResolution = 128;
	// water normal map resolution
	const unsigned int waterNomralMapResolution = 256;

	void UpdateCameras();
	void UpdateRippleSimulation();

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
	void SetupWaterSurface(AssetImporter& importer);
	void SetupLights(AssetImporter& importer, SceneObjectRef playerObject);

	std::vector<SceneObjectRef>& GetPointLights();
};

#endif
