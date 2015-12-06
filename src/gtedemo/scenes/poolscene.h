#ifndef _GTE_POOLSCENE_H_
#define _GTE_POOLSCENE_H_

#include <vector>

#include "engine.h"
#include "gtedemo/scene.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "base/bitmask.h"
#include "graphics/materialvardirectory.h"


class PoolScene : public Scene
{
	// the SceneObject instance at the root of the scene
	GTE::SceneObjectSharedPtr sceneRoot;
	// point lights in this scene
	std::vector<GTE::SceneObjectSharedPtr> pointLights;
	// global directional light
	GTE::SceneObjectSharedPtr directionalLightObject;
	// player object
	GTE::SceneObjectSharedPtr playerObject;
	// position of player in previous frame
	GTE::Point3 lastPlayerPos;
	// main camera
	GTE::CameraSharedPtr mainCamera;

	// water surface object
	GTE::SceneObjectSharedPtr waterSurfaceSceneObject;
	// camera for rendering water reflection
	GTE::CameraSharedPtr waterReflectionCamera;
	// camera for rendering water's surface
	GTE::CameraSharedPtr waterSurfaceCamera;
	// material for rendering water
	GTE::MaterialSharedPtr waterMaterial;
	// height maps for water surface
	GTE::RenderTargetSharedPtr waterHeights[2];
	// material for rendering the height maps
	GTE::MaterialSharedPtr waterHeightsMaterial;
	// material for rendering the normal map
	GTE::MaterialSharedPtr waterNormalsMaterial;
	// material for adding a new water drop
	GTE::MaterialSharedPtr waterDropMaterial;

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

	// custom material uniforms
	GTE::UniformID uniform_WATER_HEIGHT_MAP;
	GTE::UniformID uniform_PIXEL_DISTANCE;
	GTE::UniformID uniform_REFRACTED_COLOR_FACTOR;
	GTE::UniformID uniform_REFLECTED_COLOR_FACTOR;
	GTE::UniformID uniform_SELFCOLOR;
	GTE::UniformID uniform_REFLECTED_TEXTURE;
	GTE::UniformID uniform_SCREEN_BUFFER_TEXTURE;
	GTE::UniformID uniform_DROP_RADIUS;
	GTE::UniformID uniform_DROP_STRENGTH;
	GTE::UniformID uniform_DROP_POSITION;

	void UpdateCameras();
	void UpdateRippleSimulation();

public:

	PoolScene();
	~PoolScene();

	GTE::SceneObjectSharedPtr GetSceneRoot();
	void OnActivate();
	void Update();
	void Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject, GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject);
	void SetMainCamera(GTE::CameraSharedPtr camera);

	void SetupTerrain(GTE::AssetImporter& importer);
	void SetupStructures(GTE::AssetImporter& importer);
	void SetupPlants(GTE::AssetImporter& importer);
	void SetupWaterSurface(GTE::AssetImporter& importer);
	void SetupExtra(GTE::AssetImporter& importer);
	void SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject);

	void TriggerRippleDrop();

	std::vector<GTE::SceneObjectSharedPtr>& GetPointLights();
};

#endif
