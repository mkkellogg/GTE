#ifndef _GTE_POOLSCENE_H_
#define _GTE_POOLSCENE_H_

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
	GTE::SceneObjectRef sceneRoot;
	// point lights in this scene
	std::vector<GTE::SceneObjectRef> pointLights;
	// global directional light
	GTE::SceneObjectRef directionalLightObject;
	// player object
	GTE::SceneObjectRef playerObject;
	// position of player in previous frame
	GTE::Point3 lastPlayerPos;
	// main camera
	GTE::CameraRef mainCamera;

	// water surface object
	GTE::SceneObjectRef waterSurfaceSceneObject;
	// camera for rendering water reflection
	GTE::CameraRef waterReflectionCamera;
	// camera for rendering water's surface
	GTE::CameraRef waterSurfaceCamera;
	// material for rendering water
	GTE::MaterialRef waterMaterial;
	// height maps for water surface
	GTE::RenderTargetRef waterHeights[2];
	// normal maps for water surface
	GTE::RenderTargetRef waterNormals;
	// material for rendering the height maps
	GTE::MaterialRef waterHeightsMaterial;
	// material for rendering the normal map
	GTE::MaterialRef waterNormalsMaterial;
	// material for adding a new water drop
	GTE::MaterialRef waterDropMaterial;

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

	GTE::SceneObjectRef GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectRef ambientLightObject, GTE::SceneObjectRef directionalLightObject, GTE::SceneObjectRef playerObject);
	void SetMainCamera(GTE::CameraRef camera);

	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupPlants(GTE::AssetImporter& importer);
	void SetupWaterSurface(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject);

	std::vector<GTE::SceneObjectRef>& GetPointLights();
};

#endif
