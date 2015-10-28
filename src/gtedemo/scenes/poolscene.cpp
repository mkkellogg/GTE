#include <functional>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include "engine.h"
#include "input/inputmanager.h"
#include "gtedemo/gameutil.h"
#include "poolscene.h"
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/material.h"
#include "graphics/render/rendermanager.h"
#include "graphics/light/light.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/shader/shadersource.h"
#include "base/basevector4.h"
#include "geometry/matrix4x4.h"
#include "geometry/quaternion.h"
#include "geometry/transform.h"
#include "geometry/sceneobjecttransform.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/point/point3array.h"
#include "debug/gtedebug.h"
#include "object/engineobjectmanager.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "object/layermanager.h"
#include "util/time.h"
#include "util/engineutility.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"
#include "filesys/filesystem.h"

/*
 * Constructor - initialize member variables.
 */
PoolScene::PoolScene() : Scene()
{
	currentHeightMapIndex = 0;
	shouldTripperDrop = false;
	lastWaterDropTime = GTE::Time::GetRealTimeSinceStartup();
	lastWaterSimAdvanceTime = GTE::Time::GetRealTimeSinceStartup();
}

/*
 * Clean up.
 */
PoolScene::~PoolScene()
{

}

/*
 * Get the SceneObject instance at the root of the scene.
 */
GTE::SceneObjectRef PoolScene::GetSceneRoot()
{
	return sceneRoot;
}

/*
 * Called whenever this scene is activated.
 */
void PoolScene::OnActivate()
{
	//directionalLightObject->GetLight()->SetDirection(.8,-1.7,2);
	//Engine::Instance()->GetRenderManager()->ClearCaches();

	UpdateCameras();
}

/*
 * Update the positions of [waterReflectionCamera] and [waterSurfaceCamera].
 * Additionally set reflection parameters for [waterReflectionCamera].
 */
void PoolScene::UpdateCameras()
{
	GTE::Transform mainCameraTrans;
	GTE::SceneObjectTransform::GetWorldTransform(mainCameraTrans, mainCamera->GetSceneObject(), true, false);

	// set the water reflection camera's position & orientation to that of [mainCamera]
	sceneRoot->RemoveChild(waterReflectionCamera->GetSceneObject());
	waterReflectionCamera->GetSceneObject()->GetTransform().SetTo(mainCameraTrans);
	sceneRoot->AddChild(waterReflectionCamera->GetSceneObject());

	// set the water surface camera's position & orientation to that of [mainCamera]
	sceneRoot->RemoveChild(waterSurfaceCamera->GetSceneObject());
	waterSurfaceCamera->GetSceneObject()->GetTransform().SetTo(mainCameraTrans);
	sceneRoot->AddChild(waterSurfaceCamera->GetSceneObject());

	waterReflectionCamera->SetReverseCulling(true);

	// get full world-space transformation for water's surface
	GTE::Transform waterSurfaceTransform;
	GTE::SceneObjectTransform::GetWorldTransform(waterSurfaceTransform, waterSurfaceSceneObject, true, false);
	GTE::Transform waterSurfaceInverseTransform = waterSurfaceTransform;
	waterSurfaceInverseTransform.Invert();

	// build the transform that will reflect the scene about the water's surface
	// NOTE: Since these are pre-transformations the implicit assumption is that
	// in the reflection plane's local space the scale is uniformly 1, and the reflection plane 
	// is parallel to the XZ plane
	GTE::Transform reflectionTransform;
	reflectionTransform.PreTransformBy(waterSurfaceInverseTransform);
	reflectionTransform.Scale(1, -1, 1, false);
	reflectionTransform.PreTransformBy(waterSurfaceTransform);

	// tell [waterReflectionCamera] about the reflection transform
	waterReflectionCamera->SetUniformWorldSceneObjectTransform(reflectionTransform);

	// need to reflect skybox texture coordinates
	GTE::Transform skyboxTrans;
	skyboxTrans.Scale(1, -1, 1, false);
	waterReflectionCamera->SetSkyboxTextureTransform(skyboxTrans);
}

/*
* Trigger addition of drop to ripple simulation
*/
void PoolScene::TriggerRippleDrop()
{
	shouldTripperDrop = true;
}

/*
 * Advance the ripple simulation by one frame (if enough time has elapsed).
 */
void PoolScene::UpdateRippleSimulation()
{
	GTE::Graphics* graphics = GTE::Engine::Instance()->GetGraphicsSystem();

	// calculate the time difference between ripple simulation frames
	GTE::Real simAdvanceTimeDiff = GTE::Time::GetRealTimeSinceStartup() - lastWaterSimAdvanceTime;

	// make sure the ripple simulation runs at a max of 60 iterations per second
	if(simAdvanceTimeDiff > simFrameTime)
	{
		GTE::RenderManager * renderManager = GTE::Engine::Instance()->GetRenderManager();
		GTE::UInt32 renderHeightMap = 0;

		// add water drop if enough time has passed since the last drop
		if(GTE::Time::GetRealTimeSinceStartup() - lastWaterDropTime > waterDropFrequency || shouldTripperDrop)
		{
			shouldTripperDrop = false;
			// calculate drop position and drop size
			GTE::Real dropRadius = 8.0f / (GTE::Real)waterHeightMapResolution * ((((GTE::Real)rand() / (GTE::Real)RAND_MAX) * 0.7f) + 0.3f);
			GTE::Real x = 1.6f * (GTE::Real)rand() / (GTE::Real)RAND_MAX - .8f;
			GTE::Real y = .8f - 1.6f * (GTE::Real)rand() / (GTE::Real)RAND_MAX;

			GTE::Real dropStrength = 2.3f;

			dropStrength = (((GTE::Real)rand() / (GTE::Real)RAND_MAX) * 1.0f) + 0.5f;
			dropRadius = dropStrength / 35.0f;

			//dropStrength = 0.8f;
			//dropRadius = .05f;

		//	dropStrength = 2.0f;
			//dropRadius = .05f;

			dropStrength = 0.4f;
			dropRadius = .04f;

			// set variable values in [waterDropMaterial]			
			waterDropMaterial->SetUniform1f(dropRadius, "DROP_RADIUS");
			waterDropMaterial->SetUniform1f(dropStrength, "DROP_STRENGTH");
			waterDropMaterial->SetUniform2f(x * 0.5f + 0.5f, 0.5f - y * 0.5f, "DROP_POSITION");

			// render water drop to water height map
			waterDropMaterial->SetTexture(waterHeights[currentHeightMapIndex]->GetColorTexture(), "WATER_HEIGHT_MAP");
			renderHeightMap = (currentHeightMapIndex + 1) % 2;
			renderManager->RenderFullScreenQuad(waterHeights[renderHeightMap], waterDropMaterial, false);
			graphics->RebuildMipMaps(waterHeights[renderHeightMap]->GetColorTexture());
			++currentHeightMapIndex %= 2;

			lastWaterDropTime = GTE::Time::GetRealTimeSinceStartup();
		}

		// update water height map
		waterHeightsMaterial->SetTexture(waterHeights[currentHeightMapIndex]->GetColorTexture(), "WATER_HEIGHT_MAP");
		renderHeightMap = (currentHeightMapIndex + 1) % 2;
		renderManager->RenderFullScreenQuad(waterHeights[renderHeightMap], waterHeightsMaterial, false);
		graphics->RebuildMipMaps(waterHeights[renderHeightMap]->GetColorTexture());
		++currentHeightMapIndex %= 2;

		// update water normal map
		waterNormalsMaterial->SetTexture(waterHeights[currentHeightMapIndex]->GetColorTexture(), "WATER_HEIGHT_MAP");
		renderHeightMap = (currentHeightMapIndex + 1) % 2;
		renderManager->RenderFullScreenQuad(waterHeights[renderHeightMap], waterNormalsMaterial, false);
		graphics->RebuildMipMaps(waterHeights[renderHeightMap]->GetColorTexture());
		++currentHeightMapIndex %= 2;

		GTE::Transform mainCameraTransform;
		GTE::SceneObjectTransform::GetWorldTransform(mainCameraTransform, mainCamera->GetSceneObject(), true, false);

		GTE::Point3 cameraPos;
		mainCameraTransform.TransformPoint(cameraPos);
		//waterMaterial->SetUniform4f(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f, "CAMERA_POSITION");
		waterMaterial->SetTexture(waterHeights[renderHeightMap]->GetColorTexture(), "WATER_HEIGHT_MAP");

		if(pointLights[0]->IsActive())
		{
			waterMaterial->SetUniform1f(.3f, "REFLECTED_COLOR_FACTOR");
			waterMaterial->SetUniform1f(.7f, "REFRACTED_COLOR_FACTOR");
		}
		else
		{
			waterMaterial->SetUniform1f(.8f, "REFLECTED_COLOR_FACTOR");
			waterMaterial->SetUniform1f(.2f, "REFRACTED_COLOR_FACTOR");
		}

		while(GTE::Time::GetRealTimeSinceStartup() - lastWaterSimAdvanceTime > simFrameTime)
		{
			lastWaterSimAdvanceTime += simFrameTime;
		}
	}
}
/*
 * Update() is called once per frame from the Game() instance.
 */
void PoolScene::Update()
{
	if(waterReflectionCamera.IsValid() && waterReflectionCamera->GetSceneObject().IsValid())
	{
		// Update the positions of [waterReflectionCamera] and [waterSurfaceCamera] to match [mainCamera].
		// Additionally set reflection parameters for [waterReflectionCamera].
		UpdateCameras();

		// advance ripples
		UpdateRippleSimulation();
	}
}

/*
 * Set up all elements of the scene using [importer] to load any assets from disk.
 *
 * [ambientLightObject] - Global scene object that contains the global ambient light.
 * [directLightObject] - Global scene object that contains the global directional light.
 * [playerObject] - Scene object that contains the player mesh & renderer.
 */
void PoolScene::Setup(GTE::AssetImporter& importer, GTE::SceneObjectRef ambientLightObject, GTE::SceneObjectRef directionalLightObject, GTE::SceneObjectRef playerObject)
{
	importer.SetBoolProperty(GTE::AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT(sceneRoot.IsValid(), "Could not create scene root for pool scene!\n");

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupPlants(importer);
	SetupExtra(importer);
	SetupWaterSurface(importer);
	SetupLights(importer, playerObject);

	this->directionalLightObject = directionalLightObject;
	this->playerObject = playerObject;
}

/*
 * Set the reference to the main/global camera.
 */
void PoolScene::SetMainCamera(GTE::CameraRef camera)
{
	mainCamera = camera;
}

/*
* Set up the "land" elements in the scene.
*/
void PoolScene::SetupTerrain(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Islands
	//
	//========================================================

	// load castle island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1, false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load island model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 45, true);
	modelSceneObject->GetTransform().Scale(.06f, .06f, .06f, false);

	sceneRoot->GetTransform().Translate(-7, -10, 14, false);
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void PoolScene::SetupStructures(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Pool components
	//
	//========================================================

	// load castle tower for pool corners
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_02.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load tower model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle tower model
	GTE::SceneObjectRef tower2MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef tower2Mesh = tower2MeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef towerRenderer = tower2MeshObject->GetMesh3DRenderer();
	GTE::MaterialRef towerMaterial = towerRenderer->GetMaterial(0);

	// place initial castle tower in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04f, .015f, .04f, false);
	modelSceneObject->GetTransform().Translate(4.5f, -10, 10, false);

	// re-use the castle tower mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04f, .04f, .015f, 1, 0, 0, -90, 4.5f, -10, 31.5f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04f, .04f, .015f, 1, 0, 0, -90, -16.5f, -10, 10, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04f, .04f, .015f, 1, 0, 0, -90, -16.5f, -10, 31.5f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);


	// load single stone model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/StoneAlone1.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load single stone model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle tower model
	GTE::SceneObjectRef singleStoneMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef singleStoneMesh = singleStoneMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef singleStoneRenderer = singleStoneMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef singleStoneMaterial = singleStoneRenderer->GetMaterial(0);

	// place initial single stone in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.05f, .025f, .10f, false);
	modelSceneObject->GetTransform().Translate(2.5f, -6.5, 13.2f, false);

	// re-use the single stone mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, 2.5f, -6.5, 18.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, 2.5f, -6.5, 23.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, 2.5f, -6.5, 28.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, -15.0f, -6.5, 13.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, -15.0f, -6.5, 18.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, -15.0f, -6.5, 23.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .05f, .10f, .025f, 1, 0, 0, -90, -15.0f, -6.5, 28.2f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .11f, .065f, .025f, 1, 0, 0, -90, -1.0f, -6.5, 11.7f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .11f, .065f, .025f, 1, 0, 0, -90, -6.5f, -6.5, 11.7f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .11f, .065f, .025f, 1, 0, 0, -90, -12.0f, -6.5, 11.7f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .11f, .065f, .025f, 1, 0, 0, -90, -1.0f, -6.5, 30.2f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .11f, .065f, .025f, 1, 0, 0, -90, -6.5f, -6.5, 30.2f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(singleStoneMesh, singleStoneMaterial, .11f, .065f, .025f, 1, 0, 0, -90, -12.0f, -6.5, 30.2f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);

	singleStoneMesh->GetSubMesh(0)->SetCalculateNormals(true);
	singleStoneMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	singleStoneMesh->Update();


	// load castle wall model for pool walls
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Block_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle wall model
	GTE::SceneObjectRef wallBlockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef wallBlockRenderer = wallBlockMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef wallBlockMaterial = wallBlockRenderer->GetMaterial(0);

	// place initial pool wall in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.06f, .025f, .04f, false);
	modelSceneObject->GetTransform().Translate(-10, -10, 9.5f, false);

	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06f, .04f, .025f, 1, 0, 0, -90, -1.8f, -10, 9.5f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);

	// re-use the castle wall mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04f, .06f, .025f, 1, 0, 0, -90, -17, -10, 16.5f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04f, .06f, .025f, 1, 0, 0, -90, -17, -10, 25, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06f, .04f, .025f, 1, 0, 0, -90, -1.8f, -10, 31.7f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06f, .04f, .025f, 1, 0, 0, -90, -10, -10, 31.7f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04f, .06f, .025f, 1, 0, 0, -90, 4.9f, -10, 16.5f, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04f, .06f, .025f, 1, 0, 0, -90, 4.9f, -10, 25, true, true, true);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	sceneRoot->AddChild(modelSceneObject);


	// load castle wall model for pool floor
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Block_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	wallBlockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	wallBlockMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(5);
	wallBlockMesh->Update();

	// place pool floor in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Rotate(1, 0, 0, -90, true);
	modelSceneObject->GetTransform().Scale(.175f, .175f, .175f, false);
	modelSceneObject->GetTransform().Translate(-5.5f, -17.5f, 33, false);
}

/*
* Add all the plants to the scene.
*/
void PoolScene::SetupPlants(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Trees
	//
	//========================================================

	// load tree model
	modelSceneObject = importer.LoadModelDirect("resources/models/toontree/toontree2/treeplain.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load tree model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	GameUtil::SetAllMeshesShadowVolumeOffset(modelSceneObject, 2.0f);

	// extract tree mesh & material
	GTE::SceneObjectRef treeMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef treeMesh = treeMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef treeRenderer = treeMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef treeMaterial = treeRenderer->GetMaterial(0);


	// place initial tree in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0015f, .0015f, .0015f, false);
	//modelSceneObject->GetTransform().Rotate(.8f, 0, .2f, -6, false);
	modelSceneObject->GetTransform().Translate(-8, -10.5f, 40, false);

	// reuse the tree mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .10f, .10f, .10f, 1, 0, 0, -85, -4, -10, 49, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	GameUtil::SetAllMeshesShadowVolumeOffset(modelSceneObject, 2.0f);

	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .15f, .15f, .15f, 1, 0, 0, -100, 0, -7, 15, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	GameUtil::SetAllMeshesShadowVolumeOffset(modelSceneObject, 2.0f);

	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .20f, .20f, .20f, 1, 0, 0, -85, 15, -12, 25, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	GameUtil::SetAllMeshesShadowVolumeOffset(modelSceneObject, 2.0f);

	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .20f, .20f, .24f, 1, 0, 0, -85, -27, -10, 25, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	GameUtil::SetAllMeshesShadowVolumeOffset(modelSceneObject, 2.0f);

	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .18f, .18f, .20f, 1, 0, 0, -100, -22, -7, 18, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	GameUtil::SetAllMeshesShadowVolumeOffset(modelSceneObject, 2.0f);

}

/*
* Add miscellaneous elements to the scene.
*/
void PoolScene::SetupExtra(GTE::AssetImporter& importer)
{
	// misc. reference variables
	GTE::SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Fences
	//
	//========================================================

	// load fence model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrier01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load fence model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract fence mesh & material
	GTE::SceneObjectRef fenceMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef fenceMesh = fenceMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef fenceRenderer = fenceMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef fenceMaterial = fenceRenderer->GetMaterial(0);

	// place initial fence in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 50, false);
	modelSceneObject->GetTransform().Scale(.6f, .6f, .6f, false);
	modelSceneObject->GetTransform().Translate(6, -10.5f, 42, false);

	/** re-use the fence mesh & material for multiple instances  **/
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, -1.0f, -10.5f, 49.0f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 40, true);

	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, -10.0f, -10.5f, 50.0f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -25, true);

	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 23.0f, -10.5f, 15.0f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 60, true);

	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 23.0f, -10.5f, 6.0f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 120, true);
}

/*
* Add water surface to the scene.
*/
void PoolScene::SetupWaterSurface(GTE::AssetImporter& importer)
{
	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	GTE::Graphics* graphics = GTE::Engine::Instance()->GetGraphicsSystem();
	const GTE::GraphicsAttributes& graphicsAttr = graphics->GetAttributes();

	// create layer for water's surface
	unsigned reflectiveLayerIndex = objectManager->GetLayerManager().AddLayer("Reflective");
	GTE::IntMask reflectiveMask = objectManager->GetLayerManager().GetLayerMask(reflectiveLayerIndex);

	// prevent [main camera] from rendering the water surface's mesh
	GTE::IntMask cameraMask = mainCamera->GetCullingMask();
	cameraMask = objectManager->GetLayerManager().RemoveLayerFromMask(cameraMask, reflectiveLayerIndex);
	mainCamera->SetCullingMask(cameraMask);


	//========================================================
	//
	// Set up camera that will render water reflection
	//
	//========================================================

	waterReflectionCamera = objectManager->CreateCamera();
	waterReflectionCamera->SetSSAOEnabled(false);
	waterReflectionCamera->AddClearBuffer(GTE::RenderBufferType::Color);
	waterReflectionCamera->AddClearBuffer(GTE::RenderBufferType::Depth);
	// ensure [waterReflectionCamera] renders before [mainCamera]
	waterReflectionCamera->SetRenderOrderIndex(0);
	waterReflectionCamera->SetupOffscreenRenderTarget(graphicsAttr.WindowWidth, graphicsAttr.WindowHeight, false);
	waterReflectionCamera->ShareSkybox(mainCamera);
	waterReflectionCamera->SetSkyboxEnabled(true);
	waterReflectionCamera->SetWidthHeightRatio((GTE::Real)graphicsAttr.WindowWidth, (GTE::Real)graphicsAttr.WindowHeight);

	// prevent [waterReflectionCamera] from rendering the water surface's mesh
	cameraMask = waterReflectionCamera->GetCullingMask();
	cameraMask = objectManager->GetLayerManager().RemoveLayerFromMask(cameraMask, reflectiveLayerIndex);
	waterReflectionCamera->SetCullingMask(cameraMask);

	GTE::SceneObjectRef waterReflectionCameraObject = objectManager->CreateSceneObject();
	waterReflectionCameraObject->SetCamera(waterReflectionCamera);
	sceneRoot->AddChild(waterReflectionCameraObject);
	waterReflectionCamera->AddClipPlane(GTE::Vector3(0, -1, 0), -7);


	//========================================================
	//
	// Set up camera that will render water's surface
	//
	//========================================================

	waterSurfaceCamera = objectManager->CreateCamera();
	waterSurfaceCamera->SetSSAOEnabled(false);
	waterSurfaceCamera->SetAmbientPassEnabled(false);
	waterSurfaceCamera->SetDepthPassEnabled(false);
	// ensure [waterSurfaceCamera] renders after [mainCamera]
	waterSurfaceCamera->SetRenderOrderIndex(10);

	// restrict [waterSurfaceCamera] to rendering only the water's surface
	cameraMask = objectManager->GetLayerManager().GetLayerMask(reflectiveLayerIndex);
	waterSurfaceCamera->SetCullingMask(cameraMask);

	GTE::SceneObjectRef waterSurfaceCameraObject = objectManager->CreateSceneObject();
	waterSurfaceCameraObject->SetCamera(waterSurfaceCamera);
	sceneRoot->AddChild(waterSurfaceCameraObject);

	//========================================================
	//
	// Create scene object for the water's surface
	//
	//========================================================

	waterSurfaceSceneObject = objectManager->CreateSceneObject();

	// set the water surface's layer
	waterSurfaceSceneObject->SetLayerMask(reflectiveMask);

	// create mesh for water's surface
	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture0);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture1);
	GTE::Mesh3DRef waterMesh = GTE::EngineUtility::CreateRectangularMesh(meshAttributes, 2, 2, waterMeshResolution, waterMeshResolution, false, false, false);

	// create material for water surface
	GTE::ShaderSource waterShaderSource;
	importer.LoadBuiltInShaderSource("waterplanar", waterShaderSource);
	waterMaterial = objectManager->CreateMaterial("WaterMaterial", waterShaderSource);
	waterMaterial->SetSelfLit(true);
	waterMaterial->SetTexture(waterReflectionCamera->GetRenderTarget()->GetColorTexture(), "REFLECTED_TEXTURE");
	waterMaterial->SetTexture(mainCamera->GetCopyRenderTarget()->GetColorTexture(), "SCREEN_BUFFER_TEXTURE");

	// create a renderer for the water mesh
	GTE::Mesh3DRendererRef waterMeshRenderer = objectManager->CreateMesh3DRenderer();
	waterMeshRenderer->AddMaterial(waterMaterial);
	waterSurfaceSceneObject->SetMesh3DRenderer(waterMeshRenderer);

	// apply rotation of -90 degrees around positive x-axis to water mesh
	GTE::Transform rot90;
	rot90.Rotate(1, 0, 0, -90, true);
	for(GTE::UInt32 i = 0; i < waterMesh->GetSubMesh(0)->GetPostions()->GetCount(); i++)
	{
		GTE::Point3 * p = waterMesh->GetSubMesh(0)->GetPostions()->GetPoint(i);
		rot90.TransformPoint(*p);
		p->y = 0;
	}

	// add water mesh to its scene object
	GTE::Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	waterSurfaceSceneObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(waterMesh);
	filter->SetCastShadows(false);
	filter->SetReceiveShadows(false);

	// set the size & position of the water's surface
	waterSurfaceSceneObject->GetTransform().Scale(9.75f, 1, 9.75f, false);
	waterSurfaceSceneObject->GetTransform().Translate(-6.05f, -7, 20.6f, false);
	sceneRoot->AddChild(waterSurfaceSceneObject);


	//========================================================
	//
	// Create materials and render targets for processing ripple effects
	//
	//========================================================

	GTE::TextureAttributes renderTargetColorAttributes;
	renderTargetColorAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	renderTargetColorAttributes.MipMapLevel = 8;
	renderTargetColorAttributes.WrapMode = GTE::TextureWrap::Clamp;
	renderTargetColorAttributes.Format = GTE::TextureFormat::RGBA32F;

	waterHeights[0] = objectManager->CreateRenderTarget(true, false, false, renderTargetColorAttributes, waterHeightMapResolution, waterHeightMapResolution);
	waterHeights[1] = objectManager->CreateRenderTarget(true, false, false, renderTargetColorAttributes, waterHeightMapResolution, waterHeightMapResolution);

	ASSERT(waterHeights[0].IsValid() && waterHeights[1].IsValid(), "PoolScene::SetupWaterSurface -> Could not create render target for water height map.");

	GTE::UInt32 mapSize = waterHeightMapResolution * waterHeightMapResolution * 4;
	GTE::Real * heightData = new(std::nothrow) GTE::Real[mapSize];
	ASSERT(heightData != nullptr, "PoolScene::SetupWaterSurface -> Could not allocate initialization data for water height map.");

	// create water height map initialization data
	for(GTE::UInt32 i = 0; i < mapSize; i += 4)
	{
		heightData[i] = 0;
		heightData[i + 1] = 0;
		heightData[i + 2] = 0;
		heightData[i + 3] = 0;
	}

	// initialize water height maps
	graphics->SetTextureData(waterHeights[0]->GetColorTexture(), (GTE::Byte *)heightData);
	graphics->SetTextureData(waterHeights[1]->GetColorTexture(), (GTE::Byte *)heightData);

	delete heightData;

	// create material for adding water drops to height maps
	GTE::ShaderSource waterDropShaderSource;
	importer.LoadBuiltInShaderSource("waterdrop", waterDropShaderSource);
	waterDropMaterial = objectManager->CreateMaterial("WaterDropMaterial", waterDropShaderSource);
	waterDropMaterial->SetSelfLit(true);
	waterDropMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");

	// create material for updating height maps
	GTE::ShaderSource waterHeightsShaderSource;
	importer.LoadBuiltInShaderSource("waterheights", waterHeightsShaderSource);
	waterHeightsMaterial = objectManager->CreateMaterial("WaterHeightsMaterial", waterHeightsShaderSource);
	waterHeightsMaterial->SetSelfLit(true);
	waterHeightsMaterial->SetUniform1f(1.0f / (GTE::Real)waterHeightMapResolution, "PIXEL_DISTANCE");
	waterHeightsMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");

	// create material for updating water normal map
	GTE::ShaderSource waterNormalsShaderSource;
	importer.LoadBuiltInShaderSource("waternormals", waterNormalsShaderSource);
	waterNormalsMaterial = objectManager->CreateMaterial("WaterNormalsMaterial", waterNormalsShaderSource);
	waterNormalsMaterial->SetSelfLit(true);
	waterNormalsMaterial->SetUniform1f(1.0f / (GTE::Real)waterHeightMapResolution, "PIXEL_DISTANCE");
	waterNormalsMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");

	// set appropriate sampler uniforms for water surface shader
	waterMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");
	//waterMaterial->SetUniform1f(1.0 / (GTE::Real)waterNomralMapResolution, "PIXEL_DISTANCE");
	waterMaterial->SetUniform1f(.8f, "REFLECTED_COLOR_FACTOR");
	waterMaterial->SetUniform1f(.2f, "REFRACTED_COLOR_FACTOR");
	waterMaterial->SetUniform1f(1.0f / (GTE::Real)waterHeightMapResolution, "PIXEL_DISTANCE");

	GTE::Transform mainCameraTransform;
	GTE::SceneObjectTransform::GetWorldTransform(mainCameraTransform, mainCamera->GetSceneObject(), true, false);
	GTE::Point3 cameraPos;
	mainCameraTransform.TransformPoint(cameraPos);
	//waterMaterial->SetUniform4f(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f, "CAMERA_POSITION");
}

/*
* Set up the lights that belong to this scene.
*/
void PoolScene::SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject)
{
	GTE::SceneObjectRef sceneObject;

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	// create material for light meshes
	GTE::ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	GTE::MaterialRef selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	selflitMaterial->SetColor(GTE::Color4(1, 1, 1, 1), "SELFCOLOR");

	//========================================================
	//
	// Pool light
	//
	//========================================================

	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);

	// create mesh & material for pool light
	GTE::Mesh3DRef poolLightMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Color4 poolLightColor(1, .66f, .231f, 1);
	GTE::Color4 poolLightMeshColor(1, .95f, .5f, 1);
	GTE::MaterialRef poolLightMeshMaterial = objectManager->CreateMaterial("LanternLightMeshMaterial", selfLitShaderSource);
	poolLightMeshMaterial->SetColor(poolLightMeshColor, "SELFCOLOR");
	poolLightMeshMaterial->SetSelfLit(true);

	// create pool light
	GTE::SceneObjectRef poolLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(poolLightObject);
	poolLightObject->SetStatic(true);
	GTE::LightRef poolLight = objectManager->CreateLight();
	poolLight->SetIntensity(1.8f);
	poolLight->SetRange(25);
	poolLight->SetColor(poolLightColor);
	poolLight->SetShadowsEnabled(true);
	poolLight->SetType(GTE::LightType::Point);
	poolLightObject->SetLight(poolLight);
	poolLightObject->GetTransform().Scale(.2f, .2f, .2f, true);
	poolLightObject->GetTransform().Translate(0, 0, 20, false);

	// set up culling mask for pool light
	GTE::IntMask mergedMask = poolLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	poolLight->SetCullingMask(mergedMask);

	// add mesh for pool light to scene
	GTE::Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	poolLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(poolLightMesh);
	GTE::Mesh3DRendererRef lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(poolLightMeshMaterial);
	poolLightObject->SetMesh3DRenderer(lanterLightRenderer);

	pointLights.push_back(poolLightObject);
}

/*
 * Return the point lights used in this scene.
 */
std::vector<GTE::SceneObjectRef>& PoolScene::GetPointLights()
{
	return pointLights;
}
