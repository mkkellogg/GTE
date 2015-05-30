#include <functional>
#include <vector>
#include "lavascene.h"
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
#include "global/constants.h"
#include "gtemath/gtemath.h"
#include "filesys/filesystem.h"

/*
 * Constructor - initialize member variables.
 */
PoolScene::PoolScene() : Scene()
{
	currentHeightMapIndex = 0;
	lastWaterDropTime = Time::GetRealTimeSinceStartup() ;
	lastWaterSimAdvanceTime = Time::GetRealTimeSinceStartup() ;
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
SceneObjectRef PoolScene::GetSceneRoot()
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
	Transform mainCameraTrans;
	SceneObjectTransform::GetWorldTransform(mainCameraTrans , mainCamera->GetSceneObject(), true ,false);

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
	Transform waterSurfaceTransform;
	SceneObjectTransform::GetWorldTransform(waterSurfaceTransform, waterSurfaceSceneObject, true ,false);
	Transform waterSurfaceInverseTransform = waterSurfaceTransform;
	waterSurfaceInverseTransform.Invert();

	// build the transform that will reflect the scene about the water's surface,
	// which is the XZ plane in the object's local space.
	Transform reflectionTransform;
	reflectionTransform.PreTransformBy(waterSurfaceInverseTransform);
	reflectionTransform.Scale(1,-1,1, false);
	reflectionTransform.PreTransformBy(waterSurfaceTransform);

	// tell [waterReflectionCamera] about the reflection transform
	waterReflectionCamera->SetUniformWorldSceneObjectTransform(reflectionTransform);

	// need to reflect skybox texture coordinates
	Transform skyboxTrans;
	skyboxTrans.Scale(1,-1,1,false);
	waterReflectionCamera->SetSkyboxTextureTransform(skyboxTrans);
}

/*
 * Advance the ripple simulation by one frame (if enough time has elapsed).
 */
void PoolScene::UpdateRippleSimulation()
{
	Graphics* graphics = Engine::Instance()->GetGraphicsSystem();

	// calculate the time difference between ripple simulation frames
	float frameTime  = 1.0/60.0;
	float simAdvanceTimeDiff = Time::GetRealTimeSinceStartup() - lastWaterSimAdvanceTime;

	// make sure the ripple simulation runs at a max of 60 iterations per second
	if(simAdvanceTimeDiff > frameTime)
	{
		RenderManager * renderManager = Engine::Instance()->GetRenderManager();
		unsigned int renderHeightMap = 0;

		// add water drop if enough time has passed since the last drop
		if(Time::GetRealTimeSinceStartup() - lastWaterDropTime > .6)
		{
			// calculate drop position and drop size
			float dropRadius = 4.0f / (float)waterHeightMapResolution * (float)rand() / (float)RAND_MAX;
			float x = 2.0f * (float)rand() / (float)RAND_MAX - 1.0f;
			float y = 1.0f - 2.0f * (float)rand() / (float)RAND_MAX;

			// set variable values in [waterDropMaterial]
			waterDropMaterial->SetTexture(waterHeights[currentHeightMapIndex]->GetColorTexture(), "WATER_HEIGHT_MAP");
			waterDropMaterial->SetUniform1f(dropRadius, "DROP_RADIUS");
			waterDropMaterial->SetUniform2f(x * 0.5f + 0.5f, 0.5f - y * 0.5f, "DROP_POSITION");
			renderHeightMap = (currentHeightMapIndex + 1) % 2;

			// render water drop to water height map
			renderManager->RenderFullScreenQuad(waterHeights[renderHeightMap], waterDropMaterial, false);
			graphics->RebuildMipMaps(waterHeights[renderHeightMap]->GetColorTexture());

			++currentHeightMapIndex %= 2;
			lastWaterDropTime = Time::GetRealTimeSinceStartup();
		}

		// update water height map
		waterHeightsMaterial->SetTexture(waterHeights[currentHeightMapIndex]->GetColorTexture(), "WATER_HEIGHT_MAP");
		renderHeightMap = (currentHeightMapIndex + 1) % 2;
		renderManager->RenderFullScreenQuad(waterHeights[renderHeightMap], waterHeightsMaterial, false);
		graphics->RebuildMipMaps(waterHeights[renderHeightMap]->GetColorTexture());
		++currentHeightMapIndex %= 2;

		// update water normal map
		waterNormalsMaterial->SetTexture(waterHeights[renderHeightMap]->GetColorTexture(), "WATER_HEIGHT_MAP");
		renderManager->RenderFullScreenQuad(waterNormals, waterNormalsMaterial, false);
		graphics->RebuildMipMaps(waterNormals->GetColorTexture());

		Transform mainCameraTransform;
		SceneObjectTransform::GetWorldTransform(mainCameraTransform , mainCamera->GetSceneObject(), true ,false);

		Point3 cameraPos;
		mainCameraTransform.TransformPoint(cameraPos);
		waterMaterial->SetTexture(waterHeights[renderHeightMap]->GetColorTexture(), "WATER_HEIGHT_MAP");
		//waterMaterial->SetUniform4f(cameraPos.x, cameraPos.y, cameraPos.z, 1, "CAMERA_POSITION");

		if(pointLights[0]->IsActive())
		{
			waterMaterial->SetUniform1f(.3, "REFLECTED_COLOR_FACTOR");
			waterMaterial->SetUniform1f(.7, "REFRACTED_COLOR_FACTOR");
		}
		else
		{
			waterMaterial->SetUniform1f(.8, "REFLECTED_COLOR_FACTOR");
			waterMaterial->SetUniform1f(.2, "REFRACTED_COLOR_FACTOR");
		}

		while(Time::GetRealTimeSinceStartup() - lastWaterSimAdvanceTime > frameTime)
		{
			lastWaterSimAdvanceTime += frameTime;
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
void PoolScene::Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directionalLightObject, SceneObjectRef playerObject)
{
	importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT(sceneRoot.IsValid(), "Could not create scene root for pool scene!\n");

	SetupTerrain(importer);
	SetupStructures(importer);
	//SetupPlants(importer);
	SetupWaterSurface(importer);
	SetupLights(importer,playerObject);

	this->directionalLightObject = directionalLightObject;
	this->playerObject = playerObject;
}

/*
 * Set the reference to the main/global camera.
 */
void PoolScene::SetMainCamera(CameraRef camera)
{
	mainCamera = camera;
}

/*
* Set up the "land" elements in the scene.
*/
void PoolScene::SetupTerrain(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Islands
	//
	//========================================================

	// load castle island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load island model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Rotate(0,1,0,45, true);
	modelSceneObject->GetTransform().Scale(.06,.06,.06, false);

	sceneRoot->GetTransform().Translate(-7,-10,14,false);
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void PoolScene::SetupStructures(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

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
	SceneObjectRef tower2MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef tower2Mesh = tower2MeshObject->GetMesh3D();
	Mesh3DRendererRef towerRenderer = tower2MeshObject->GetMesh3DRenderer();
	MaterialRef towerMaterial = towerRenderer->GetMaterial(0);

	// place initial castle tower in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.015,.04, false);
	modelSceneObject->GetTransform().Translate(4.5,-10,10,false);

	// re-use the castle tower mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.015, 1,0,0, -90, 4.5,-10,31.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.015, 1,0,0, -90, -16.5,-10,10, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.015, 1,0,0, -90, -16.5,-10,31.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);

	// load castle wall model for pool walls
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Block_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle wall model
	SceneObjectRef wallBlockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	Mesh3DRendererRef wallBlockRenderer = wallBlockMeshObject->GetMesh3DRenderer();
	MaterialRef wallBlockMaterial = wallBlockRenderer->GetMaterial(0);

	// place initial pool wall in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.06,.025,.04, false);
	modelSceneObject->GetTransform().Translate(-10,-10,9.5,false);

	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.025, 1,0,0, -90, -1.8, -10, 9.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);

	// re-use the castle wall mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.06,.025, 1,0,0, -90, -17,-10, 16.5, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.06,.025, 1,0,0, -90, -17,-10,25, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90, true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.025, 1,0,0, -90, -1.8, -10, 31.7, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.025, 1,0,0, -90, -10, -10, 31.7, true,true,true);
	sceneRoot->AddChild(modelSceneObject);

	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.06,.025, 1,0,0, -90, 4.9,-10, 16.5, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.06,.025, 1,0,0, -90, 4.9, -10,25, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90, true);
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
	modelSceneObject->GetTransform().Rotate(1,0,0,-90, true);
	modelSceneObject->GetTransform().Scale(.175,.175,.175, false);
	modelSceneObject->GetTransform().Translate(-5.5,-17.5,33,false);
}

/*
* Add all the plants to the scene.
*/
void PoolScene::SetupPlants(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

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

	// extract tree mesh & material
	SceneObjectRef treeMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef treeMesh = treeMeshObject->GetMesh3D();
	Mesh3DRendererRef treeRenderer = treeMeshObject->GetMesh3DRenderer();
	MaterialRef treeMaterial = treeRenderer->GetMaterial(0);

	// place initial tree in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0015,.0015,.0015, false);
	modelSceneObject->GetTransform().Rotate(.8,0,.2, -6, false);
	modelSceneObject->GetTransform().Translate(55,-10.5, 11,false);

	// reuse the tree mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .10,.10,.10, 1,0,0, -85, 57, -10, 24, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .15,.15,.20, 1,0,0, -94, 61, -9, -15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .20,.20,.30, 1,0,0, -93, 80, -9, -15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .17,.17,.20, 1,0,0, -85, 85, -9.5, -13, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .22,.22,.38, 1,0,0, -90, 115, -9.5, 15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .19,.19,.28, 1,0,0, -96, 105, -9.5, 8, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .18,.18,.20, 1,0,0, -87, 95, -10, 32, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
}

/*
* Add water surface to the scene.
*/
void PoolScene::SetupWaterSurface(AssetImporter& importer)
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	Graphics* graphics = Engine::Instance()->GetGraphicsSystem();
	const GraphicsAttributes& graphicsAttr = graphics->GetAttributes();

	// create layer for water's surface
	unsigned reflectiveLayerIndex = objectManager->GetLayerManager().AddLayer("Reflective");
	IntMask reflectiveMask = objectManager->GetLayerManager().GetLayerMask(reflectiveLayerIndex);

	// prevent [main camera] from rendering the water surface's mesh
	IntMask cameraMask = mainCamera->GetCullingMask();
	cameraMask = objectManager->GetLayerManager().RemoveLayerFromMask(cameraMask, reflectiveLayerIndex);
	mainCamera->SetCullingMask(cameraMask);


	//========================================================
	//
	// Set up camera that will render water reflection
	//
	//========================================================

	waterReflectionCamera = objectManager->CreateCamera();
	waterReflectionCamera->SetSSAOEnabled(false);
	waterReflectionCamera->AddClearBuffer(RenderBufferType::Color);
	waterReflectionCamera->AddClearBuffer(RenderBufferType::Depth);
	// ensure [waterReflectionCamera] renders before [mainCamera]
	waterReflectionCamera->SetRenderOrderIndex(0);
	waterReflectionCamera->SetupOffscreenRenderTarget(graphicsAttr.WindowWidth,graphicsAttr.WindowHeight,false);
	waterReflectionCamera->ShareSkybox(mainCamera);
	waterReflectionCamera->SetSkyboxEnabled(true);
	waterReflectionCamera->SetWidthHeightRatio(graphicsAttr.WindowWidth,graphicsAttr.WindowHeight);

	// prevent [waterReflectionCamera] from rendering the water surface's mesh
	cameraMask = waterReflectionCamera->GetCullingMask();
	cameraMask = objectManager->GetLayerManager().RemoveLayerFromMask(cameraMask, reflectiveLayerIndex);
	waterReflectionCamera->SetCullingMask(cameraMask);

	// set up camera that will render water reflection
	waterSurfaceCamera = objectManager->CreateCamera();
	waterSurfaceCamera->SetSSAOEnabled(false);
	// ensure [waterSurfaceCamera] renders after [mainCamera]
	waterSurfaceCamera->SetRenderOrderIndex(10);

	// restrict [waterSurfaceCamera] to rendering only the water's surface
	cameraMask = objectManager->GetLayerManager().GetLayerMask(reflectiveLayerIndex);
	waterSurfaceCamera->SetCullingMask(cameraMask);

	SceneObjectRef waterReflectionCameraObject = objectManager->CreateSceneObject();
	waterReflectionCameraObject->SetCamera(waterReflectionCamera);
	sceneRoot->AddChild(waterReflectionCameraObject);
	waterReflectionCamera->AddClipPlane(Vector3(0,-1,0), -7);

	SceneObjectRef waterSurfaceCameraObject = objectManager->CreateSceneObject();
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
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture1);
	Mesh3DRef waterMesh = EngineUtility::CreateRectangularMesh(meshAttributes, 2,2,waterMeshResolution,waterMeshResolution, false, false, false);

	// create material for water surface
	ShaderSource waterShaderSource;
	importer.LoadBuiltInShaderSource("waterplanar", waterShaderSource);
	waterMaterial = objectManager->CreateMaterial("WaterMaterial", waterShaderSource);
	waterMaterial->SetSelfLit(true);
	waterMaterial->SetTexture(waterReflectionCamera->GetRenderTarget()->GetColorTexture(), "REFLECTED_TEXTURE");
	waterMaterial->SetTexture(mainCamera->GetCopyRenderTarget()->GetColorTexture(), "SCREEN_BUFFER_TEXTURE");

	// create a renderer for the water mesh
	Mesh3DRendererRef waterMeshRenderer = objectManager->CreateMesh3DRenderer();
	waterMeshRenderer->AddMaterial(waterMaterial);
	waterSurfaceSceneObject->SetMesh3DRenderer(waterMeshRenderer);

	// apply rotation of 90 degrees around positive x-axis to water mesh
	Transform rot90;
	rot90.Rotate(1,0,0,-90, true);
	for(unsigned int i =0; i <  waterMesh->GetSubMesh(0)->GetPostions()->GetCount();i++)
	{
		Point3 * p = waterMesh->GetSubMesh(0)->GetPostions()->GetPoint(i);
		rot90.TransformPoint(*p);
		p->y = 0;
	}

	// add water mesh to its scene object
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	waterSurfaceSceneObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(waterMesh);
	filter->SetCastShadows(false);
	filter->SetReceiveShadows(false);

	// set the size & position of the water's surface
	waterSurfaceSceneObject->GetTransform().Scale(9.75,1,9.75,false);
	waterSurfaceSceneObject->GetTransform().Translate(-6.05,-7,20.6,false);
	sceneRoot->AddChild(waterSurfaceSceneObject);


	//========================================================
	//
	// Create materials and render targets for processing ripple effects
	//
	//========================================================

	TextureAttributes renderTargetColorAttributes;
	renderTargetColorAttributes.FilterMode = TextureFilter::TriLinear;
	renderTargetColorAttributes.MipMapLevel = 8;
	renderTargetColorAttributes.WrapMode = TextureWrap::Clamp;
	renderTargetColorAttributes.Format = TextureFormat::RGBA32F;

	waterHeights[0] = objectManager->CreateRenderTarget(true,false,false,renderTargetColorAttributes,waterHeightMapResolution,waterHeightMapResolution);
	waterHeights[1] = objectManager->CreateRenderTarget(true,false,false,renderTargetColorAttributes,waterHeightMapResolution,waterHeightMapResolution);
	waterNormals = objectManager->CreateRenderTarget(true,false,false,renderTargetColorAttributes,waterNomralMapResolution,waterNomralMapResolution);

	ASSERT(waterHeights[0].IsValid() && waterHeights[1].IsValid(), "PoolScene::SetupWaterSurface -> Could not create render target for water height map.");
	ASSERT(waterNormals.IsValid(), "PoolScene::SetupWaterSurface -> Could not create render target for water normals map.");

	unsigned int mapSize = waterHeightMapResolution * waterHeightMapResolution * 4;
	float * heightData = new float[mapSize];
	ASSERT(heightData != NULL, "PoolScene::SetupWaterSurface -> Could not allocate initialization data for water height map.");

	// create water height map initialization data
	for(unsigned int i = 0; i < mapSize; i += 4)
	{
		heightData[i] = 0;
		heightData[i+1] = 0;
		heightData[i+2] = 0;
		heightData[i+3] = 0;
	}

	mapSize = waterNomralMapResolution * waterNomralMapResolution * 4;
	float * normalData = new float[mapSize];
	ASSERT(normalData != NULL, "PoolScene::SetupWaterSurface -> Could not allocate initialization data for water normal map.");

	// create water normal map initialization data
	for(unsigned int i = 0; i < mapSize; i += 4)
	{
		normalData[i] = 0;
		normalData[i+1] = 0;
		normalData[i+2] = 1;
		normalData[i+3] = 1;
	}

	// initialize water height maps
	graphics->SetTextureData(waterHeights[0]->GetColorTexture(), (BYTE *)heightData);
	graphics->SetTextureData(waterHeights[1]->GetColorTexture(), (BYTE *)heightData);

	// initialize water normal maps
	graphics->SetTextureData(waterNormals->GetColorTexture(), (BYTE *)normalData);

	delete heightData;
	delete normalData;

	// create material for adding water drops to height maps
	ShaderSource waterDropShaderSource;
	importer.LoadBuiltInShaderSource("waterdrop", waterDropShaderSource);
	waterDropMaterial = objectManager->CreateMaterial("WaterDropMaterial", waterDropShaderSource);
	waterDropMaterial->SetSelfLit(true);
	waterDropMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");

	// create material for updating height maps
	ShaderSource waterHeightsShaderSource;
	importer.LoadBuiltInShaderSource("waterheights", waterHeightsShaderSource);
	waterHeightsMaterial = objectManager->CreateMaterial("WaterHeightsMaterial", waterHeightsShaderSource);
	waterHeightsMaterial->SetSelfLit(true);
	waterHeightsMaterial->SetUniform1f(1.0 / (float)waterHeightMapResolution, "PIXEL_DISTANCE");
	waterHeightsMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");

	// create material for updating water normal map
	ShaderSource waterNormalsShaderSource;
	importer.LoadBuiltInShaderSource("waternormals", waterNormalsShaderSource);
	waterNormalsMaterial = objectManager->CreateMaterial("WaterNormalsMaterial", waterNormalsShaderSource);
	waterNormalsMaterial->SetSelfLit(true);
	waterNormalsMaterial->SetUniform1f(1.0 / (float)waterNomralMapResolution, "PIXEL_DISTANCE");
	waterNormalsMaterial->SetUniform1f(2.0 / (float)waterNomralMapResolution * 2.0, "PIXEL_DISTANCEX2");
	waterNormalsMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");

	// set appropriate sampler uniforms for water surface shader
	waterMaterial->SetTexture(waterHeights[0]->GetColorTexture(), "WATER_HEIGHT_MAP");
	waterMaterial->SetTexture(waterNormals->GetColorTexture(), "WATER_NORMAL_MAP");
	//waterMaterial->SetUniform1f(1.0 / (float)waterNomralMapResolution, "PIXEL_DISTANCE");
	waterMaterial->SetUniform1f(.8, "REFLECTED_COLOR_FACTOR");
	waterMaterial->SetUniform1f(.2, "REFRACTED_COLOR_FACTOR");
}

/*
* Set up the lights that belong to this scene.
*/
void PoolScene::SetupLights(AssetImporter& importer, SceneObjectRef playerObject)
{
	SceneObjectRef sceneObject;

	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// create material for light meshes
	ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	MaterialRef selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	selflitMaterial->SetColor(Color4(1,1,1,1), "SELFCOLOR");

	//========================================================
	//
	// Pool light
	//
	//========================================================

	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	// create mesh & material for pool light
	Mesh3DRef poolLightMesh = EngineUtility::CreateCubeMesh(meshAttributes);
	Color4 poolLightColor(1,.66,.231,1);
	Color4 poolLightMeshColor(1,.95,.5,1);
	MaterialRef poolLightMeshMaterial = objectManager->CreateMaterial("LanternLightMeshMaterial", selfLitShaderSource);
	poolLightMeshMaterial->SetColor(poolLightMeshColor, "SELFCOLOR");
	poolLightMeshMaterial->SetSelfLit(true);

	// create pool light
	SceneObjectRef poolLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(poolLightObject);
	poolLightObject->SetStatic(true);
	LightRef poolLight = objectManager->CreateLight();
	poolLight->SetIntensity(1.8);
	poolLight->SetRange(25);
	poolLight->SetColor(poolLightColor);
	poolLight->SetShadowsEnabled(true);
	poolLight->SetType(LightType::Point);
	poolLightObject->SetLight(poolLight);
	poolLightObject->GetTransform().Scale(.2,.2,.2, true);
	poolLightObject->GetTransform().Translate(0, 0, 20, false);

	// set up culling mask for pool light
	IntMask mergedMask = poolLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	poolLight->SetCullingMask(mergedMask);

	// add mesh for pool light to scene
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	poolLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(poolLightMesh);
	Mesh3DRendererRef lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(poolLightMeshMaterial);
	poolLightObject->SetMesh3DRenderer(lanterLightRenderer);

	pointLights.push_back(poolLightObject);
}

/*
 * Return the point lights used in this scene.
 */
std::vector<SceneObjectRef>& PoolScene::GetPointLights()
{
	return pointLights;
}
