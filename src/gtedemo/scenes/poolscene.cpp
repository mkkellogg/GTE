//forward declarations
class AssetImporter;
class Vector3;
class Quaternion;
class LavaField;

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
}


/*
 * Update() is called once per frame from the Game() instance.
 */
void PoolScene::Update()
{
	if(waterCamera.IsValid() && waterCamera->GetSceneObject().IsValid())
	{
		Point3 reflectCamPos;
		Point3 poolCenter(-5,0,15);

		Vector3 trans, scale;
		Quaternion rot;

		waterCamera->GetSceneObject()->GetTransform().SetTo(mainCamera->GetSceneObject()->GetAggregateTransform());
		//waterCamera->GetSceneObject()->GetTransform().Scale(1,-1,1,true);
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
	ASSERT_RTRN(sceneRoot.IsValid(), "Could not create scene root for pool scene!\n");

	SetupTerrain(importer);
	SetupStructures(importer);
	//SetupPlants(importer);
	SetupExtra(importer);
	SetupLights(importer,playerObject);

	//sceneRoot->GetTransform().Rotate(0,1,0, 155, true);
	//sceneRoot->GetTransform().Translate(-44.3, 0, 52, false);

	this->directionalLightObject = directionalLightObject;
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
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load island model!\n");
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
	// Castle components
	//
	//========================================================

	// load castle tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_02.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tower model!\n");
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

	// load castle wall model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Block_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle wall model
	SceneObjectRef wallBlockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	Mesh3DRendererRef wallBlockRenderer = wallBlockMeshObject->GetMesh3DRenderer();
	MaterialRef wallBlockMaterial = wallBlockRenderer->GetMaterial(0);

	// place initial castle wall in scene
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
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tree model!\n");
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
* Add miscellaneous elements to the scene.
*/
void PoolScene::SetupExtra(AssetImporter& importer)
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	waterObject = objectManager->CreateSceneObject();

	unsigned reflectiveLayerIndex = objectManager->GetLayerManager().AddLayer("Reflective");
	IntMask reflectiveMask = objectManager->GetLayerManager().GetLayerMask(reflectiveLayerIndex);
	waterObject->SetLayerMask(reflectiveMask);

	// set up camera that will render water reflection
	waterCamera = objectManager->CreateCamera();
	waterCamera->SetSSAOEnabled(false);
	waterCamera->AddClearBuffer(RenderBufferType::Color);
	waterCamera->AddClearBuffer(RenderBufferType::Depth);
	waterCamera->SetRendeOrderIndex(0);
	waterCamera->SetupOffscreenRenderTarget(1280,800);
	waterCamera->ShareSkybox(mainCamera);
	waterCamera->SetSkyboxEnabled(true);

	IntMask cameraMask = waterCamera->GetCullingMask();
	cameraMask = objectManager->GetLayerManager().RemoveLayerFromMask(cameraMask, reflectiveLayerIndex);
	waterCamera->SetCullingMask(cameraMask);

	SceneObjectRef waterCameraObject = objectManager->CreateSceneObject();
	waterCameraObject->SetCamera(waterCamera);

	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture1);
	Mesh3DRef waterMesh = EngineUtility::CreateRectangularMesh(meshAttributes, 1,1,1,1);

	// create material water surface
	ShaderSource waterShaderSource;
	importer.LoadBuiltInShaderSource("water", waterShaderSource);
	waterMaterial = objectManager->CreateMaterial("WaterMaterial", waterShaderSource);
	waterMaterial->SetColor(Color4(1,1,1,1), "WATERCOLOR");
	waterMaterial->SetSelfLit(true);
	waterMaterial->SetTexture(waterCamera->GetRenderTarget()->GetColorTexture(), "REFLECTED_TEXTURE");

	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	waterObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(waterMesh);
	filter->SetCastShadows(false);
	filter->SetReceiveShadows(false);
	Mesh3DRendererRef waterMeshRenderer = objectManager->CreateMesh3DRenderer();
	waterMeshRenderer->AddMaterial(waterMaterial);
	waterObject->SetMesh3DRenderer(waterMeshRenderer);

	waterObject->GetTransform().Rotate(1,0,0,-90, true);
	waterObject->GetTransform().Scale(20,1,20,false);
	waterObject->GetTransform().Translate(-5,-7,20,false);
	waterObject->GetTransform().Rotate(0,0,1,-90, true);
	sceneRoot->AddChild(waterObject);

	waterCameraObject->GetTransform().Translate(-5,-7,20,false);
	sceneRoot->AddChild(waterCameraObject);

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

	// create castle right lantern
	SceneObjectRef poolLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(poolLightObject);
	poolLightObject->SetStatic(true);
	LightRef poolLight = objectManager->CreateLight();
	poolLight->SetIntensity(1.8);
	poolLight->SetRange(25);
	poolLight->SetColor(poolLightColor);
	IntMask mergedMask = poolLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	poolLight->SetCullingMask(mergedMask);
	poolLight->SetShadowsEnabled(true);
	poolLight->SetType(LightType::Point);
	poolLightObject->SetLight(poolLight);
	poolLightObject->GetTransform().Scale(.2,.2,.2, true);
	poolLightObject->GetTransform().Translate(0, 0, 20, false);
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	poolLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(poolLightMesh);
	Mesh3DRendererRef lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(poolLightMeshMaterial);
	poolLightObject->SetMesh3DRenderer(lanterLightRenderer);
	pointLights.push_back(poolLightObject);
}

std::vector<SceneObjectRef>& PoolScene::GetPointLights()
{
	return pointLights;
}
