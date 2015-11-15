#include <functional>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>

#include "lavascene.h"
#include "gtedemo/lavafield.h"
#include "object/enginetypes.h"
#include "object/engineobject.h"
#include "global/global.h"
#include "global/constants.h"
#include "engine.h"
#include "global/assert.h"
#include "input/inputmanager.h"
#include "gtedemo/gameutil.h"
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/material.h"
#include "graphics/light/light.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/shader/shadersource.h"
#include "graphics/particles/particlesystem.h"
#include "graphics/particles/particlemodifier.h"
#include "graphics/particles/randommodifier.h"
#include "graphics/particles/framesetmodifier.h"
#include "graphics/particles/evenintervalindexmodifier.h"
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
#include "gtemath/gtemath.h"
#include "filesys/filesystem.h"

const std::string LavaScene::LavaWallLayer = "LavaWall";
const std::string LavaScene::LavaIslandObjectsLayer = "LavaIslandObjects";

/*
 * Constructor - initialize member variables.
 */
LavaScene::LavaScene() : Scene()
{
	lavaField = nullptr;

	// initialize layer masks
	lavaPlanarLayerMask = 0;
	lavaIslandObjectsLayerMask = 0;
	playerObjectLayerMask = 0;

	planarLitMaterial = GTE::MaterialSharedPtr::Null();
}

/*
 * Clean up.
 */
LavaScene::~LavaScene()
{

}

/*
 * Get the SceneObject instance at the root of the scene.
 */
GTE::SceneObjectSharedPtr LavaScene::GetSceneRoot()
{
	return sceneRoot;
}

/*
 * Called whenever this scene is activated.
 */
void LavaScene::OnActivate()
{

}

/*
 * Update() is called once per frame from the Game() instance.
 */
void LavaScene::Update()
{
	GTE::Point3 lightRotatePoint(-23, 5, 0);
	spinningPointLightObject->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z, 0, 1, 0, 30 * GTE::Time::GetDeltaTime(), false);

	// rotate the cube around its center and the y-axis
	cubeSceneObject->GetTransform().Rotate(0, 1, 0, 20 * GTE::Time::GetDeltaTime(), true);

	if(lavaField->GetSceneObject()->IsActive())lavaField->Update();
}

/*
 * Set up all elements of the scene using [importer] to load any assets from disk.
 *
 * [ambientLightObject] - Global scene object that contains the global ambient light.
 * [directLightObject] - Global scene object that contains the global directional light.
 * [playerObject] - Scene object that contains the player mesh & renderer.
 */
void LavaScene::Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject, GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject)
{
	importer.SetBoolProperty(GTE::AssetImporterBoolProperty::PreserveFBXPivots, false);

	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	GTE::IntMask mergedMask;
	GTE::LayerManager& layerManager = objectManager->GetLayerManager();
	GTE::Int32 lavaWallLayerIndex = layerManager.AddLayer(LavaWallLayer);
	GTE::Int32 lavaIslandObjectsLayerIndex = layerManager.AddLayer(LavaIslandObjectsLayer);
	lavaPlanarLayerMask = layerManager.GetLayerMask(lavaWallLayerIndex);
	lavaIslandObjectsLayerMask = layerManager.GetLayerMask(lavaIslandObjectsLayerIndex);

	GTE::LightSharedPtr ambientLight = ambientLightObject->GetLight();
	mergedMask = ambientLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandObjectsLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaPlanarLayerMask);
	ambientLight->SetCullingMask(mergedMask);

	GTE::LightSharedPtr directionalLight = directionalLightObject->GetLight();
	mergedMask = directionalLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandObjectsLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaPlanarLayerMask);
	directionalLight->SetCullingMask(mergedMask);

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT(sceneRoot.IsValid(), "Could not create scene root for lava scene!\n");

	sceneRoot->GetTransform().Translate(-20, 0, 0, false);

	this->directionalLightObject = directionalLightObject;

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupExtra(importer);
	SetupLights(importer, playerObject);
	SetupParticleSystems(importer);
}

/*
 * Set the culling mask the corresponds to the player object.
 */
void LavaScene::SetPlayerObjectLayerMask(GTE::IntMask playerObjectLayerMask)
{
	this->playerObjectLayerMask = playerObjectLayerMask;
}

/*
* Set up the "land" elements in the scene.
*/
void LavaScene::SetupTerrain(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectSharedPtr modelSceneObject;

	//========================================================
	//
	// Islands
	//
	//========================================================

	// load lava island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1, false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load island model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.02f, .02f, .02f, false);
	modelSceneObject->GetTransform().Translate(-30, -10, 12, false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, -30, true);

	// load lava island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1, false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load island model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.015f, .025f, .015f, false);
	modelSceneObject->GetTransform().Translate(-17, -6.5, -16, false);

	//========================================================
	//
	// Lava
	//
	//========================================================

	lavaField = new(std::nothrow) LavaField(30);
	ASSERT(lavaField != nullptr, "Could not allocate lava field!\n");

	lavaField->Init();
	lavaField->SetDisplacementSpeed(.05f);
	lavaField->SetTextureASpeed(.010f);
	lavaField->SetTextureBSpeed(.035f);
	lavaField->SetDisplacementHeight(6);
	lavaField->SetDisplacementTileSize(3);

	GTE::SceneObjectSharedPtr lavaFieldObject = lavaField->GetSceneObject();
	sceneRoot->AddChild(lavaFieldObject);
	lavaFieldObject->GetTransform().Scale(100, 1, 97, false);
	lavaFieldObject->GetTransform().Translate(-40, -17, 0, false);


	//========================================================
	//
	// Lava wall
	//
	//========================================================

	// load stone3
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/stone/Stone03.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load stone model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);

	// extract mesh & material from stone model
	GTE::SceneObjectSharedPtr stone1MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr stone1Mesh = stone1MeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr stone1Renderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(stone1MeshObject->GetRenderer());
	GTE::MaterialSharedPtr stone1Material = stone1Renderer->GetMaterial(0);

	// place initial stone in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0045f, .0045f, .0045f, false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 25, false);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 15, false);
	modelSceneObject->GetTransform().Translate(5, -15, -45, false);

	// re-use the stone mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5f, .5f, .5f, 1, 0, 0, -90, -16, -15, -55, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5f, .5f, .5f, 0, 1, 0, -70, -54, -30, -52, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -15, false);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5f, .5f, .5f, 1, 0, 0, -90, -66, -15, -27, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -90, true);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5f, .5f, .5f, 0, 1, 0, -20, -95, -17, 8, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .55f, .58f, .55f, 1, 0, 0, -90, -90, -15, 30, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -40, true);
	modelSceneObject->GetTransform().Rotate(1, 0, 0, -180, true);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5f, .5f, .5f, 0, 1, 0, -80, -85, -28, 43, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .45f, .4f, .45f, 1, 0, 0, -90, -40, -25, 45, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -40, true);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .6f, .5f, .5f, 0, 1, 0, -80, -20, -28, 45, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .40f, .6f, .55f, 0, 1, 0, -10, 10, -30, 21, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .25f, .6f, .35f, 0, 1, 0, -180, 10, -25, -27, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .40f, .6f, .40f, 0, 1, 0, -180, 17, -32, 15, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaPlanarLayerMask);
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void LavaScene::SetupStructures(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectSharedPtr modelSceneObject;

	//========================================================
	//
	// Island structures
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/mushroom/MushRoom_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load mushroom house model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// extract mesh & material from mushroom house model
	GTE::SceneObjectSharedPtr mushroomHouseMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr mushroomHouseMesh = mushroomHouseMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr mushroomHouseMeshRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(mushroomHouseMeshObject->GetRenderer());
	GTE::Mesh3DFilterSharedPtr mushroomHouseMeshFilter = mushroomHouseMeshObject->GetMesh3DFilter();

	// place mushroom house in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.12f, .12f, .12f, false);
	modelSceneObject->GetTransform().Translate(-19, -6.5, -15, false);

	// load turrent tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load tower model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place turret tower in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.05f, .05f, .05f, false);
	modelSceneObject->GetTransform().Translate(-28.5, -10, 7.5, false);

	//========================================================
	//
	// wood bridge
	//
	//========================================================

	// load bridge
	modelSceneObject = importer.LoadModelDirect("resources/models/bridge/bridge.fbx", 1, true, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load bridge model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	GTE::SceneObjectSharedPtr woodBridgeMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DFilterSharedPtr woodBridgeMeshFilter = woodBridgeMeshObject->GetMesh3DFilter();
	GTE::Mesh3DSharedPtr woodBridgeMesh = woodBridgeMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr woodBridgeRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(woodBridgeMeshObject->GetRenderer());
	GTE::MaterialSharedPtr woodBridgeMaterial = woodBridgeRenderer->GetMaterial(0);
	woodBridgeMeshFilter->SetUseBackSetShadowVolume(false);
	woodBridgeMeshFilter->SetUseCustomShadowVolumeOffset(true);
	woodBridgeMeshFilter->SetCustomShadowVolumeOffset(0.6f);

	// place bridge in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.1f, .1f, .075f, false);
	modelSceneObject->GetTransform().Translate(-20, -11.5f, 27, false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 20, true);

	modelSceneObject = GameUtil::AddMeshToScene(woodBridgeMesh, woodBridgeMaterial, .06f, .1f, .1f, 1, 0, 0, -90, -13, -8, -46, true, true, true);
	woodBridgeMeshFilter = modelSceneObject->GetMesh3DFilter();
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
	woodBridgeMeshFilter->SetUseBackSetShadowVolume(false);
	woodBridgeMeshFilter->SetUseCustomShadowVolumeOffset(true);
	woodBridgeMeshFilter->SetCustomShadowVolumeOffset(0.6f);
}

/*
* Add miscellaneous elements to the scene.
*/
void LavaScene::SetupExtra(GTE::AssetImporter& importer)
{
	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	//========================================================
	//
	// Texture scene cube
	//
	//========================================================

	// create instance of SceneObject to hold the cube mesh and its renderer
	cubeSceneObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(cubeSceneObject);
	GameUtil::SetAllObjectsLayerMask(cubeSceneObject, lavaIslandObjectsLayerMask);

	// load texture for the cube
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	GTE::TextureSharedPtr texture = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/color.png", texAttributes);
	GTE::TextureSharedPtr normalmap = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/normal.png", texAttributes);

	// create the cube's material using the "basic" built-in shader
	GTE::ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic_normal", basicShaderSource);
	GTE::MaterialSharedPtr material = objectManager->CreateMaterial(std::string("BasicMaterial"), basicShaderSource);
	material->SetTexture(texture, "TEXTURE0");
	material->SetTexture(normalmap, "NORMALMAP");
	material->SetUniform1f(1.0f, "USCALE");
	material->SetUniform1f(1.0f, "VSCALE");
	material->SetUniform1f(1.0f, "SPECULAR_FACTOR");

	// set the cube mesh attributes
	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture0);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::VertexColor);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Tangent);

	// create the cube mesh
	GTE::Mesh3DSharedPtr cubeMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Mesh3DFilterSharedPtr cubeMeshfilter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(cubeMeshfilter);
	cubeMeshfilter->SetMesh3D(cubeMesh);
	cubeMeshfilter->SetCastShadows(true);
	cubeMeshfilter->SetReceiveShadows(true);

	// create the cube mesh's renderer
	GTE::Mesh3DRendererSharedPtr renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::Mesh3DRenderer, GTE::Renderer>(renderer));

	// scale the cube and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(1.5f, 1.5f, 1.5f, false);
	cubeSceneObject->GetTransform().Translate(-37, -7, 14, false);
}

/*
* Set up the lights that belong to this scene.
*/
void LavaScene::SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject)
{
	// multi-use reference
	GTE::SceneObjectSharedPtr modelSceneObject;

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	//========================================================
	//
	// Lava island spinning point light
	//
	//========================================================

	// create self-illuminated cube mesh to represent spinning point light
	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);
	GTE::Mesh3DSharedPtr pointLightCubeMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);

	// create material for spinning point light mesh
	GTE::ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	GTE::MaterialSharedPtr selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	selflitMaterial->SetUseLighting(false);
	selflitMaterial->SetColor(GTE::Color4(1, 1, 1, 1), "SELFCOLOR");

	// create spinning point light
	spinningPointLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(spinningPointLightObject);
	GTE::Mesh3DFilterSharedPtr filter = objectManager->CreateMesh3DFilter();
	spinningPointLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(pointLightCubeMesh);
	filter->SetCastShadows(false);
	GTE::Mesh3DRendererSharedPtr renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	spinningPointLightObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::Mesh3DRenderer, GTE::Renderer>(renderer));
	GTE::LightSharedPtr light = objectManager->CreateLight();
	light->SetIntensity(2.5f);
	GTE::IntMask mergedMask = lavaIslandObjectsLayerMask;
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	light->SetCullingMask(mergedMask);
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(GTE::LightType::Point);
	spinningPointLightObject->SetLight(light);
	spinningPointLightObject->GetTransform().Scale(.4f, .4f, .4f, true);
	spinningPointLightObject->GetTransform().Translate(4, 2, 0, false);

	//========================================================
	//
	// Lava lights
	//
	//========================================================

	// create lava pool planar light
	GTE::SceneObjectSharedPtr lavaLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lavaLightObject);
	lavaLightObject->SetStatic(true);
	GTE::LightSharedPtr lavaLight = objectManager->CreateLight();
	lavaLight->SetDirection(0, 1, 0);
	lavaLight->SetIntensity(3.25f);
	lavaLight->SetCullingMask(lavaPlanarLayerMask);
	lavaLight->SetRange(55);
	lavaLight->SetAttenuation(.05f);
	lavaLight->SetParallelAngleAttenuationType(GTE::AngleAttenuationType::Past90);
	lavaLight->SetOrthoAngleAttenuationType(GTE::AngleAttenuationType::Past90);
	lavaLight->SetColor(GTE::Color4(1, .5f, 0, 1));
	lavaLight->SetShadowsEnabled(false);
	lavaLight->SetType(GTE::LightType::Planar);
	lavaLightObject->SetLight(lavaLight);
	lavaLightObject->GetTransform().Translate(-34, -20, -6, false);
	lavaLightObjects.push_back(lavaLightObject);

	mergedMask = lavaLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObjectLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandObjectsLayerMask);
	lavaLight->SetCullingMask(mergedMask);

}

void LavaScene::SetupParticleSystems(GTE::AssetImporter& importer)
{
	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	GTE::SceneObjectRef particleSystemObject = objectManager->CreateSceneObject();
	ASSERT(particleSystemObject.IsValid(), "Unable to create flame particle system object!\n");
	sceneRoot->AddChild(particleSystemObject);

	// create material for rendering flame particles
	std::string shaderName = "particles_unlit";
	std::string materialName = "ParticlesUnlit";
	GTE::MaterialRef flameMaterial = GTE::ParticleSystem::CreateMaterial(shaderName, materialName);
	ASSERT(flameMaterial.IsValid(), "Unable to create flame material!\n");

	// load texture for the the flame's atlas
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 2;
	GTE::TextureRef flameTexture = objectManager->CreateTexture("resources/textures/particles/fireloop3.jpg", texAttributes);
	ASSERT(flameTexture.IsValid(), "Unable to load flame texture!\n");

	GTE::AtlasRef flameAtlas = objectManager->CreateGridAtlas(flameTexture, 0.0f, 1.0f, 1.0f, 0.0f, 8, 8, false, true);
	ASSERT(flameAtlas.IsValid(), "Unable to create flame atlas!\n");

	GTE::ParticleSystemRef flameSystem = objectManager->CreateParticleSystem(flameMaterial, flameAtlas, false, 3.0f, 3.0f, 0.0f);
	ASSERT(flameSystem.IsValid(), "Unable to create flame particle system!\n");
	particleSystemObject->SetParticleSystem(flameSystem);
	
	GTE::RandomModifier<GTE::Point3> positionModifier(GTE::Point3(50.0f, -5.0f, 50.0f), GTE::Point3(0.0f, 0.0f, 0.0f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::RandomModifier<GTE::Vector3> velocityModifier(GTE::Vector3(0.0f, 2.3f, 0.0f), GTE::Vector3(0.9f, 0.4f, 0.9f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::EvenIntervalIndexModifier atlasModifier(64);

	GTE::FrameSetModifier<GTE::Vector2> sizeModifier;
	sizeModifier.AddFrame(0.0f, GTE::Vector2(2.0f, 2.5f));
	sizeModifier.AddFrame(3.0f, GTE::Vector2(2.0f, 2.5f));

	GTE::FrameSetModifier<GTE::Real> alphaModifier;
	alphaModifier.AddFrame(0.0f, 0.0f);
	alphaModifier.AddFrame(0.2f, 0.3f);
	alphaModifier.AddFrame(1.2f, 1.0f);
	alphaModifier.AddFrame(2.0f, 1.0f);
	alphaModifier.AddFrame(3.0f, 0.0f);

	GTE::FrameSetModifier<GTE::Color4> colorModifier;
	colorModifier.AddFrame(0.0f, GTE::Color4(1.0f, 1.0f, 1.0f, 1.0f));
	colorModifier.AddFrame(0.2f, GTE::Color4(1.0f, 1.0f, 1.0f, 1.0f));
	colorModifier.AddFrame(3.0f, GTE::Color4(1.0f, 1.0f, 1.0f, 1.0f));

	flameSystem->BindPositionModifier(positionModifier);
	flameSystem->BindVelocityModifier(velocityModifier);
	flameSystem->BindSizeModifier(sizeModifier);
	flameSystem->BindAlphaModifier(alphaModifier);
	flameSystem->BindColorModifier(colorModifier);
	flameSystem->BindAtlasModifier(atlasModifier);
}

/*
 * Get the SceneObject instance that contains the scene's spinning point light.
 */
GTE::SceneObjectSharedPtr LavaScene::GetSpinningPointLightObject()
{
	return spinningPointLightObject;
}

/*
 * Get all light objects that provide lava illumination.
 */
std::vector<GTE::SceneObjectSharedPtr>& LavaScene::GetLavaLightObjects()
{
	return lavaLightObjects;
}

/*
 * Get a pointer to the LavaField instance for this scene.
 */
LavaField * LavaScene::GetLavaField()
{
	return lavaField;
}
