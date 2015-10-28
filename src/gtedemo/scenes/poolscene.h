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
	// material for rendering the height maps
	GTE::MaterialRef waterHeightsMaterial;
	// material for rendering the normal map
	GTE::MaterialRef waterNormalsMaterial;
	// material for adding a new water drop
	GTE::MaterialRef waterDropMaterial;

	// last time drop was added to pool
	GTE::Real lastWaterDropTime;
	// last time water simulation advanced
	GTE::Real lastWaterSimAdvanceTime;
	// current height map index
	GTE::UInt32 currentHeightMapIndex;

	// water mesh resolution
	const GTE::UInt32 waterMeshResolution = 256;
	// water height map resolution
	const GTE::UInt32 waterHeightMapResolution = 256;

	const GTE::Real simFrameTime = 1.0f / 60.0f;
	const GTE::Real waterDropFrequency = 3;

	// should we add a drop?
	bool shouldTripperDrop;

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
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject);

	void TriggerRippleDrop();

	std::vector<GTE::SceneObjectRef>& GetPointLights();
};

#endif
