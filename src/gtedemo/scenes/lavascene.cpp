#include <functional>
#include <vector>
#include "lavascene.h"
#include <iostream>
#include <memory>
#include <functional>
#include "gtedemo/lavafield.h"
#include "engine.h"
#include "input/inputmanager.h"
#include "gtedemo/gameutil.h"
#include "lavascene.h"
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

const std::string LavaScene::LavaWallLayer = "LavaWall";
const std::string LavaScene::LavaIslandLayer = "LavaIsland";
const std::string LavaScene::LavaIslandObjectsLayer = "LavaIslandObjects";

/*
 * Constructor - initialize member variables.
 */
LavaScene::LavaScene() : Scene()
{
	lavaField = NULL;

	// initialize layer masks
	lavaWallLayerMask = 0;
	lavaIslandLayerMask = 0;
	lavaIslandObjectsLayerMask = 0;
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
GTE::SceneObjectRef LavaScene::GetSceneRoot()
{
	return sceneRoot;
}

/*
 * Called whenever this scene is activated.
 */
void LavaScene::OnActivate()
{
	//directionalLightObject->GetLight()->SetDirection(-.8,-1.7,-2);
	//Engine::Instance()->GetRenderManager()->ClearCaches();
}

/*
 * Update() is called once per frame from the Game() instance.
 */
void LavaScene::Update()
{
	GTE::Point3 lightRotatePoint(-20, 5, 10);
	// rotate the point light around [lightRotatePoint]
	spinningPointLightObject->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z, 0, 1, 0, 60 * GTE::Time::GetDeltaTime(), false);
	//pointLightObject->SetActive(false);

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
void LavaScene::Setup(GTE::AssetImporter& importer, GTE::SceneObjectRef ambientLightObject, GTE::SceneObjectRef directionalLightObject, GTE::SceneObjectRef playerObject)
{
	importer.SetBoolProperty(GTE::AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	GTE::LayerManager& layerManager = objectManager->GetLayerManager();
	GTE::Int32 lavaWallLayerIndex = layerManager.AddLayer(LavaWallLayer);
	GTE::Int32 lavaIslandLayerIndex = layerManager.AddLayer(LavaIslandLayer);
	GTE::Int32 lavaIslandObjectsLayerIndex = layerManager.AddLayer(LavaIslandObjectsLayer);

	lavaWallLayerMask = layerManager.GetLayerMask(lavaWallLayerIndex);
	lavaIslandLayerMask = layerManager.GetLayerMask(lavaIslandLayerIndex);
	lavaIslandObjectsLayerMask = layerManager.GetLayerMask(lavaIslandObjectsLayerIndex);

	GTE::IntMask mergedMask;

	GTE::LightRef ambientLight = ambientLightObject->GetLight();
	mergedMask = ambientLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandObjectsLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaWallLayerMask);
	ambientLight->SetCullingMask(mergedMask);

	GTE::LightRef directionalLight = directionalLightObject->GetLight();
	mergedMask = directionalLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandObjectsLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaWallLayerMask);
	directionalLight->SetCullingMask(mergedMask);

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT(sceneRoot.IsValid(), "Could not create scene root for lava scene!\n");

	sceneRoot->GetTransform().Translate(-20,0,0, false);

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupExtra(importer);
	SetupLights(importer, playerObject);

	this->directionalLightObject = directionalLightObject;

}

/*
* Set up the "land" elements in the scene.
*/
void LavaScene::SetupTerrain(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Islands
	//
	//========================================================

	// load lava island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load island model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandLayerMask);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.04,.04, false);
	modelSceneObject->GetTransform().Translate(-30,-10,0,false);

	//========================================================
	//
	// Lava
	//
	//========================================================

	lavaField = new LavaField(30);
	lavaField->Init();
	lavaField->SetDisplacementSpeed(.05);
	lavaField->SetTextureASpeed(.015);
	lavaField->SetTextureBSpeed(.035);
	lavaField->SetDisplacementHeight(6);
	lavaField->SetDisplacementTileSize(3);

	GTE::SceneObjectRef lavaFieldObject = lavaField->GetSceneObject();
	sceneRoot->AddChild(lavaFieldObject);
	lavaFieldObject->GetTransform().Scale(100,1,97, false);
	lavaFieldObject->GetTransform().Translate(-40,-17,0, false);


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
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);

	// extract mesh & material from stone model
	GTE::SceneObjectRef stone1MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef stone1Mesh = stone1MeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef stone1Renderer = stone1MeshObject->GetMesh3DRenderer();
	GTE::MaterialRef stone1Material = stone1Renderer->GetMaterial(0);

	// place initial stone in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0045, .0045, .0045, false);
	modelSceneObject->GetTransform().Rotate(0,1,0,25, false);
	modelSceneObject->GetTransform().Rotate(0,0,1,15, false);
	modelSceneObject->GetTransform().Translate(5,-15,-45,false);

	// re-use the stone mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 1, 0,0, -90, -16,-15,-55, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 0, 1,0, -70, -54,-30,-52, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-15,false);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 1, 0,0, -90, -66,-15,-27, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-90, true);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 0, 1,0, -20, -95,-17, 8, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .55,.58, .55, 1, 0,0, -90, -90,-15, 30, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-40, true);
	modelSceneObject->GetTransform().Rotate(1,0,0,-180, true);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 0, 1,0, -80, -85,-28, 43, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .45,.4, .45, 1, 0,0, -90, -40,-25, 45, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-40, true);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .6,.5, .5, 0, 1,0, -80, -20,-28, 45, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .5,.6, .6, 0, 1,0, -180, 10,-31.5, 5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = GameUtil::AddMeshToScene(stone1Mesh, stone1Material, .25,.6, .25, 0, 1,0, -180, 10,-25, -25, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void LavaScene::SetupStructures(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Mushroom house
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/mushroom/MushRoom_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load mushroom house model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place mushroom house in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.09,.09,.09, false);
	modelSceneObject->GetTransform().Translate(-37,-10,15,false);

	// load turrent tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load tower model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place turret tower in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.05,.05,.05, false);
	modelSceneObject->GetTransform().Translate(-30,-10,-10,false);

	//========================================================
	//
	// wood bridge
	//
	//========================================================

	// load bridge
	modelSceneObject = importer.LoadModelDirect("resources/models/bridge/bridge.fbx", 1 , false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load bridge model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// place bridge in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.1,.1,.075, false);
	modelSceneObject->GetTransform().Translate(-20,-11.5,27,false);
	modelSceneObject->GetTransform().Rotate(0,1,0,20,true);
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
	GTE::TextureRef texture = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/color.png", texAttributes);
	GTE::TextureRef normalmap = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/normal.png", texAttributes);


	// create the cube's material using the "basic" built-in shader
	GTE::ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic_normal", basicShaderSource);
	GTE::MaterialRef material = objectManager->CreateMaterial(std::string("BasicMaterial"), basicShaderSource);
	material->SetTexture(texture, "TEXTURE0");
	material->SetTexture(normalmap, "NORMALMAP");
	material->SetUniform1f(1.0, "USCALE");
	material->SetUniform1f(1.0, "VSCALE");

	// set the cube mesh attributes
	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture0);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::VertexColor);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Tangent);

	// create the cube mesh
	GTE::Mesh3DRef cubeMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Mesh3DFilterRef cubeMeshfilter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(cubeMeshfilter);
	cubeMeshfilter->SetMesh3D(cubeMesh);
	cubeMeshfilter->SetCastShadows(true);
	cubeMeshfilter->SetReceiveShadows(true);

	// create the cube mesh's renderer
	GTE::Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetMesh3DRenderer(renderer);

	// scale the cube and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(1.5, 1.5,1.5, false);
	cubeSceneObject->GetTransform().Translate(-20, -7, 8, false);
}

/*
* Set up the lights that belong to thsi scene.
*/
void LavaScene::SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

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
	GTE::Mesh3DRef pointLightCubeMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);

	// create material for spinning point light mesh
	GTE::ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	GTE::MaterialRef selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	selflitMaterial->SetSelfLit(true);
	selflitMaterial->SetColor(GTE::Color4(1, 1, 1, 1), "SELFCOLOR");

	// create spinning point light
	spinningPointLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(spinningPointLightObject);
	GTE::Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	spinningPointLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(pointLightCubeMesh);
	filter->SetCastShadows(false);
	GTE::Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	spinningPointLightObject->SetMesh3DRenderer(renderer);
	GTE::LightRef light = objectManager->CreateLight();
	light->SetIntensity(1.7);
	GTE::IntMask mergedMask = lavaIslandObjectsLayerMask;
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, lavaIslandLayerMask);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	light->SetCullingMask(mergedMask);
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(GTE::LightType::Point);
	spinningPointLightObject->SetLight(light);
	spinningPointLightObject->GetTransform().Scale(.4,.4,.4, true);
	spinningPointLightObject->GetTransform().Translate(-26, 2, 10, false);

	//========================================================
	//
	// Lava lights
	//
	//========================================================

	// create lava pool wall light
	GTE::SceneObjectRef lavaLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lavaLightObject);
	lavaLightObject->SetStatic(true);
	GTE::LightRef lavaLight = objectManager->CreateLight();
	lavaLight->SetIntensity(6);
	lavaLight->SetCullingMask(lavaWallLayerMask);
	lavaLight->SetRange(60);
	lavaLight->SetColor(GTE::Color4(1, .5, 0, 1));
	lavaLight->SetShadowsEnabled(false);
	lavaLight->SetType(GTE::LightType::Point);
	lavaLightObject->SetLight(lavaLight);
	lavaLightObject->GetTransform().Translate(-25, -15, 3, false);
	lavaLightObjects.push_back(lavaLightObject);

	// create lava island light
	GTE::SceneObjectRef lavaDirectionalLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lavaDirectionalLightObject);
	lavaDirectionalLightObject->SetStatic(true);
	lavaLight = objectManager->CreateLight();
	lavaLight->SetDirection(0,1,0);
	lavaLight->SetIntensity(3);
	lavaLight->SetCullingMask(lavaIslandLayerMask);
	lavaLight->SetColor(GTE::Color4(1, .5, 0, 1));
	lavaLight->SetShadowsEnabled(false);
	lavaLight->SetType(GTE::LightType::Directional);
	lavaDirectionalLightObject->SetLight(lavaLight);
	lavaLightObjects.push_back(lavaDirectionalLightObject);
}

/*
 * Get the SceneObject instance that contains the scene's spinning point light.
 */
GTE::SceneObjectRef LavaScene::GetSpinningPointLightObject()
{
	return spinningPointLightObject;
}

/*
 * Get all light objects that provide lava illumination.
 */
std::vector<GTE::SceneObjectRef>& LavaScene::GetLavaLightObjects()
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
