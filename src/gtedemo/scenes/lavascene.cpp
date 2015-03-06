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

LavaScene::LavaScene() : Scene()
{
	lavaField = NULL;

	// initialize layer masks
	lavaWallLayerMask = 0;
	lavaIslandLayerMask = 0;
	lavaIslandObjectsLayerMask = 0;
}

LavaScene::~LavaScene()
{

}

SceneObjectRef LavaScene::GetSceneRoot()
{
	return sceneRoot;
}

void LavaScene::Update()
{
	Point3 lightRotatePoint(-20,5,10);
	// rotate the point light around [lightRotatePoint]
	spinningPointLightObject->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z,0,1,0,60 * Time::GetDeltaTime(), false);
	//pointLightObject->SetActive(false);

	// rotate the cube around its center and the y-axis
	cubeSceneObject->GetTransform().Rotate(0,1,0,20 * Time::GetDeltaTime(), true);

	if(lavaField->GetSceneObject()->IsActive())lavaField->Update();
}

void LavaScene::Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directLightObject, SceneObjectRef playerObject)
{
	importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	LayerManager& layerManager = objectManager->GetLayerManager();
	int lavaWallLayerIndex = layerManager.AddLayer(LavaWallLayer);
	int lavaIslandLayerIndex = layerManager.AddLayer(LavaIslandLayer);
	int lavaIslandObjectsLayerIndex = layerManager.AddLayer(LavaIslandObjectsLayer);

	lavaWallLayerMask = layerManager.GetLayerMask(lavaWallLayerIndex);
	lavaIslandLayerMask = layerManager.GetLayerMask(lavaIslandLayerIndex);
	lavaIslandObjectsLayerMask = layerManager.GetLayerMask(lavaIslandObjectsLayerIndex);

	LightRef ambientLight = ambientLightObject->GetLight();
	ambientLight->MergeCullingMask(lavaIslandLayerMask);
	ambientLight->MergeCullingMask(lavaIslandObjectsLayerMask);
	ambientLight->MergeCullingMask(lavaWallLayerMask);

	LightRef directionalLight = directLightObject->GetLight();
	directionalLight->MergeCullingMask(lavaIslandLayerMask);
	directionalLight->MergeCullingMask(lavaIslandObjectsLayerMask);
	directionalLight->MergeCullingMask(lavaWallLayerMask);

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT_RTRN(sceneRoot.IsValid(), "Could not create scene root for lava scene!\n");

	sceneRoot->GetTransform().Translate(-20,0,0, false);

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupExtra(importer);
	SetupLights(importer, playerObject);

}

void LavaScene::SetupTerrain(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Islands
	//
	//========================================================

	// load lava island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load island model!\n");
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

	lavaField = new LavaField(24);
	lavaField->Init();
	lavaField->SetDisplacementSpeed(.05);
	lavaField->SetTextureASpeed(.015);
	lavaField->SetTextureBSpeed(.035);
	lavaField->SetDisplacementHeight(6);
	lavaField->SetDisplacementTileSize(3);

	SceneObjectRef lavaFieldObject = lavaField->GetSceneObject();
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
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load stone model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);

	// extract mesh & material from stone model
	SceneObjectRef stone1MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef stone1Mesh = stone1MeshObject->GetMesh3D();
	Mesh3DRendererRef stone1Renderer = stone1MeshObject->GetMesh3DRenderer();
	MaterialRef stone1Material = stone1Renderer->GetMaterial(0);

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

void LavaScene::SetupStructures(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Mushroom house
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/mushroom/MushRoom_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load mushroom house model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	GameUtil::SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place mushroom house in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.09,.09,.09, false);
	modelSceneObject->GetTransform().Translate(-37,-10,15,false);

	// load turrent tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tower model!\n");
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
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load bridge model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// place bridge in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.1,.1,.075, false);
	modelSceneObject->GetTransform().Translate(-20,-11.5,27,false);
	modelSceneObject->GetTransform().Rotate(0,1,0,20,true);


}

void LavaScene::SetupExtra(AssetImporter& importer)
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

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
	TextureAttributes texAttributes;
	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	TextureRef texture = objectManager->CreateTexture("resources/textures/cartoonTex03.png", texAttributes);

	// create the cube's material using the "basic" built-in shader
	ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic", basicShaderSource);
	MaterialRef material = objectManager->CreateMaterial(std::string("BasicMaterial"), basicShaderSource);
	material->SetTexture(texture, "TEXTURE0");

	// set the cube mesh attributes
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	// create the cube mesh
	Mesh3DRef cubeMesh = EngineUtility::CreateCubeMesh(meshAttributes);
	Mesh3DFilterRef cubeMeshfilter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(cubeMeshfilter);
	cubeMeshfilter->SetMesh3D(cubeMesh);
	cubeMeshfilter->SetCastShadows(true);
	cubeMeshfilter->SetReceiveShadows(true);

	// create the cube mesh's renderer
	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetMesh3DRenderer(renderer);

	// scale the cube and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(1.5, 1.5,1.5, false);
	cubeSceneObject->GetTransform().Translate(-20, -7, 8, false);
}

void LavaScene::SetupLights(AssetImporter& importer, SceneObjectRef playerObject)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	//========================================================
	//
	// Lava island spinning point light
	//
	//========================================================

	// create self-illuminated cube mesh to represent spinning point light
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
	Mesh3DRef pointLightCubeMesh = EngineUtility::CreateCubeMesh(meshAttributes);

	// create material for spinning point light mesh
	ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	MaterialRef selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	selflitMaterial->SetSelfLit(true);
	selflitMaterial->SetColor(Color4(1,1,1,1), "SELFCOLOR");

	// create spinning point light
	spinningPointLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(spinningPointLightObject);
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	spinningPointLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(pointLightCubeMesh);
	filter->SetCastShadows(false);
	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	spinningPointLightObject->SetMesh3DRenderer(renderer);
	LightRef light = objectManager->CreateLight();
	light->SetIntensity(1.7);
	light->SetCullingMask(lavaIslandObjectsLayerMask);
	light->MergeCullingMask(lavaIslandLayerMask);
	light->MergeCullingMask(playerObject->GetLayerMask());
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Point);
	spinningPointLightObject->SetLight(light);
	spinningPointLightObject->GetTransform().Scale(.4,.4,.4, true);
	spinningPointLightObject->GetTransform().Translate(-26, 2, 10, false);

	//========================================================
	//
	// Lava lights
	//
	//========================================================

	// create lava pool wall light
	SceneObjectRef lavaLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lavaLightObject);
	lavaLightObject->SetStatic(true);
	LightRef lavaLight = objectManager->CreateLight();
	lavaLight->SetIntensity(4.5);
	lavaLight->SetCullingMask(lavaWallLayerMask);
	lavaLight->SetRange(60);
	lavaLight->SetColor(Color4(1,.5,0,1));
	lavaLight->SetShadowsEnabled(false);
	lavaLight->SetType(LightType::Point);
	lavaLightObject->SetLight(lavaLight);
	lavaLightObject->GetTransform().Translate(-25, -15, 3, false);
	lavaLightObjects.push_back(lavaLightObject);

	// create lava island light
	SceneObjectRef lavaDirectionalLightObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lavaDirectionalLightObject);
	lavaDirectionalLightObject->SetStatic(true);
	lavaLight = objectManager->CreateLight();
	lavaLight->SetDirection(0,1,0);
	lavaLight->SetIntensity(3);
	lavaLight->SetCullingMask(lavaIslandLayerMask);
	lavaLight->SetColor(Color4(1,.5,0,1));
	lavaLight->SetShadowsEnabled(false);
	lavaLight->SetType(LightType::Directional);
	lavaDirectionalLightObject->SetLight(lavaLight);
	lavaLightObjects.push_back(lavaDirectionalLightObject);
}

SceneObjectRef LavaScene::GetSpinningPointLightObject()
{
	return spinningPointLightObject;
}

std::vector<SceneObjectRef>& LavaScene::GetLavaLightObjects()
{
	return lavaLightObjects;
}

LavaField * LavaScene::GetLavaField()
{
	return lavaField;
}
