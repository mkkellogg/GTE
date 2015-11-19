#include <functional>
#include <vector>
#include "lavascene.h"
#include <iostream>
#include <memory>
#include <functional>
#include "engine.h"
#include "input/inputmanager.h"
#include "gtedemo/gameutil.h"
#include "castlescene.h"
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
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"
#include "filesys/filesystem.h"


const std::string CastleScene::SmokeLayer = "Smoke";

/*
 * Constructor - initialize member variables.
 */
CastleScene::CastleScene() : Scene()
{
	lastCampFireLightIntenistyAdjuster = 0.0f;
	lastFlickerTime = 0.0f;
}

/*
 * Clean up.
 */
CastleScene::~CastleScene()
{

}

/*
 * Get the SceneObject instance at the root of the scene.
 */
GTE::SceneObjectSharedPtr CastleScene::GetSceneRoot()
{
	return sceneRoot;
}

/*
 * Called whenever this scene is activated.
 */
void CastleScene::OnActivate()
{

}


/*
 * Update() is called once per frame from the Game() instance.
 */
void CastleScene::Update()
{
	GTE::Point3 lightRotatePoint(-14, 1, 17);
	// rotate the point light around [lightRotatePoint]
	movingLightA->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z, 0, 1, 0, 60 * GTE::Time::GetDeltaTime(), false);

	FlickerCampFireLightLight();
}

/*
 * Set up all elements of the scene using [importer] to load any assets from disk.
 *
 * [ambientLightObject] - Global scene object that contains the global ambient light.
 * [directLightObject] - Global scene object that contains the global directional light.
 * [playerObject] - Scene object that contains the player mesh & renderer.
 */
void CastleScene::Setup(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr ambientLightObject, GTE::SceneObjectSharedPtr directionalLightObject, GTE::SceneObjectSharedPtr playerObject)
{
	importer.SetBoolProperty(GTE::AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT(sceneRoot.IsValid(), "Could not create scene root for castle scene!\n");

	sceneRoot->GetTransform().Translate(50, 0, -15, false);

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupPlants(importer);
	SetupExtra(importer);
	SetupLights(importer, playerObject);
	SetupCampfire(importer, playerObject);

	sceneRoot->GetTransform().Rotate(0, 1, 0, 155, true);
	sceneRoot->GetTransform().Translate(-44.3f, 0, 52, false);

	this->directionalLightObject = directionalLightObject;
}

/*
* Set up the "land" elements in the scene.
*/
void CastleScene::SetupTerrain(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectSharedPtr modelSceneObject;

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

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
	modelSceneObject->GetTransform().Scale(.07f, .07f, .07f, false);
	modelSceneObject->GetTransform().Translate(80, -10, -10, false);

	// load shader for scaled diffuse
	GTE::ShaderSource scaledDiffuseSource;
	importer.LoadBuiltInShaderSource("diffuse_texture_scaled", scaledDiffuseSource);

	// load an alternative grass texture for the island
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	GTE::TextureSharedPtr altGrass = objectManager->CreateTexture("resources/textures/cartoonseamless/cartoonTex07a.png", texAttributes);

	// load an alternative rock texture for the island
	GTE::TextureSharedPtr altRock = objectManager->CreateTexture("resources/textures/cartoonseamless/Rock02.png", texAttributes);

	// replace the grass texture for the island
	GTE::MaterialSharedPtr scaledDiffuseIslandTop = objectManager->CreateMaterial(std::string("ScaledDiffuseIslandTop"), scaledDiffuseSource);
	GTE::SceneObjectSharedPtr islandTopMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	islandTopMeshObject = islandTopMeshObject->GetParent()->GetChildAt(1);
	GTE::Mesh3DSharedPtr islandTopMesh = islandTopMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr islandTopRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(islandTopMeshObject->GetRenderer());
	scaledDiffuseIslandTop->SetUniform2f(8.0f, 8.0f, "UV_SCALE");
	islandTopRenderer->SetMaterial(0, scaledDiffuseIslandTop);
	scaledDiffuseIslandTop->SetTexture(altGrass, "TEXTURE0");

	// replace the rock texture for the island
	GTE::MaterialSharedPtr scaledDiffuseIslandRock = objectManager->CreateMaterial(std::string("ScaledDiffuseIslandRock"), scaledDiffuseSource);
	GTE::SceneObjectSharedPtr islandRockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr islandRockMesh = islandRockMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr islandRockRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(islandRockMeshObject->GetRenderer());
	scaledDiffuseIslandRock->SetUniform2f(4.0f, 4.0f, "UV_SCALE");
	islandRockRenderer->SetMaterial(0, scaledDiffuseIslandRock);
	scaledDiffuseIslandRock->SetTexture(altRock, "TEXTURE0");
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void CastleScene::SetupStructures(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectSharedPtr modelSceneObject;

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	// load texture for the castle
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	GTE::TextureSharedPtr texture = objectManager->CreateTexture("resources/models/toonlevel/castle/Castle01.png", texAttributes);
	GTE::TextureSharedPtr normalmap = objectManager->CreateTexture("resources/models/toonlevel/castle/Castle01Normals.png", texAttributes);

	// create the castle's material using the "basic_normal" built-in shader
	GTE::ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic_normal", basicShaderSource);
	GTE::MaterialSharedPtr castleNormalMaterial = objectManager->CreateMaterial(std::string("CastleNormalMat"), basicShaderSource);
	castleNormalMaterial->SetTexture(texture, "TEXTURE0");
	castleNormalMaterial->SetTexture(normalmap, "NORMALMAP");
	castleNormalMaterial->SetUniform1f(1.0f, "USCALE");
	castleNormalMaterial->SetUniform1f(1.0f, "VSCALE");
	castleNormalMaterial->SetUniform1f(0.25f, "SPECULAR_FACTOR");

	//========================================================
	//
	// Castle Towers
	//
	//========================================================

	// load castle tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_02.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load tower model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle tower model
	GTE::SceneObjectSharedPtr tower2MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr tower2Mesh = tower2MeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr towerRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(tower2MeshObject->GetRenderer());
	GTE::Mesh3DFilterSharedPtr towerMeshFilter = tower2MeshObject->GetMesh3DFilter();
	towerMeshFilter->SetUseBackSetShadowVolume(true);

	tower2Mesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	tower2Mesh->GetSubMesh(0)->SetCalculateNormals(true);
	tower2Mesh->GetSubMesh(0)->SetCalculateTangents(true);
	tower2Mesh->UpdateAll();
	towerRenderer->SetMaterial(0, castleNormalMaterial);

	// place initial castle tower in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04f, .03f, .04f, false);
	modelSceneObject->GetTransform().Translate(65, -10, -15, false);

	// re-use the castle tower mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, castleNormalMaterial, .04f, .04f, .03f, 1, 0, 0, -90, 89, -10, -15, true, true, true);
	towerMeshFilter = modelSceneObject->GetMesh3DFilter();
	towerMeshFilter->SetUseBackSetShadowVolume(true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, castleNormalMaterial, .04f, .04f, .03f, 1, 0, 0, -90, 65, -10, 6, true, true, true);
	towerMeshFilter = modelSceneObject->GetMesh3DFilter();
	towerMeshFilter->SetUseBackSetShadowVolume(true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, castleNormalMaterial, .04f, .04f, .03f, 1, 0, 0, -90, 89, -10, 6, true, true, true);
	towerMeshFilter = modelSceneObject->GetMesh3DFilter();
	towerMeshFilter->SetUseBackSetShadowVolume(true);
	sceneRoot->AddChild(modelSceneObject);


	//========================================================
	//
	// Castle Archways
	//
	//========================================================


	// load & place castle entrance arch-way left side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Left_02.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04f, .04f, .04f, false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 90, false);
	modelSceneObject->GetTransform().Translate(65, -10, -8.8f, false);

	// extract mesh & material from left archway model
	GTE::SceneObjectSharedPtr archwayMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr archwayMesh = archwayMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr archwayRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(archwayMeshObject->GetRenderer());
	GTE::Mesh3DFilterSharedPtr archwayMeshFilter = archwayMeshObject->GetMesh3DFilter();
	archwayMeshFilter->SetUseBackSetShadowVolume(false);

	// prep left archway mesh & material for normal mapped rendering
	archwayMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	archwayMesh->GetSubMesh(0)->SetCalculateNormals(true);
	archwayMesh->GetSubMesh(0)->SetCalculateTangents(true);
	archwayMesh->UpdateAll();
	archwayRenderer->SetMaterial(0, castleNormalMaterial);
	modelSceneObject->SetActive(false);

	// load and place castle entrance arch-way right side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Right_02.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04f, .04f, .04f, false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 90, false);
	modelSceneObject->GetTransform().Translate(65, -10, 0, false);

	// extract mesh & material from right archway model
	archwayMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	archwayMesh = archwayMeshObject->GetMesh3D();
	archwayRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(archwayMeshObject->GetRenderer());
	archwayMeshFilter = archwayMeshObject->GetMesh3DFilter();
	archwayMeshFilter->SetUseBackSetShadowVolume(false);

	// prep right archway mesh & material for normal mapped rendering
	archwayMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	archwayMesh->GetSubMesh(0)->SetCalculateNormals(true);
	archwayMesh->GetSubMesh(0)->SetCalculateTangents(true);
	archwayMesh->UpdateAll();
	archwayRenderer->SetMaterial(0, castleNormalMaterial);
	modelSceneObject->SetActive(false);


	//========================================================
	//
	// Castle walls
	//
	//========================================================

	// load castle wall model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Block_01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle wall model
	GTE::SceneObjectSharedPtr wallBlockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr wallBlockRenderer =  GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(wallBlockMeshObject->GetRenderer());
	GTE::Mesh3DFilterSharedPtr wallMeshFilter = wallBlockMeshObject->GetMesh3DFilter();
	wallMeshFilter->SetUseBackSetShadowVolume(false);

	wallBlockMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	wallBlockMesh->GetSubMesh(0)->SetCalculateNormals(true);
	wallBlockMesh->GetSubMesh(0)->SetCalculateTangents(true);
	wallBlockMesh->UpdateAll();
	wallBlockRenderer->SetMaterial(0, castleNormalMaterial);

	// place initial castle wall in scene (on right side)
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.06f, .05f, .04f, false);
	modelSceneObject->GetTransform().Translate(70, -10, 6.5f, false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 180, true);

	// right side
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 78, -10, 6.5f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 86, -10, 6.5f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);

	// extra right side
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 70, -10, 10.0f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 78, -10, 10.0f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 86, -10, 10.0f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);

	// left side
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 70, -10, -15.5f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 78, -10, -15.5f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 86, -10, -15.5f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);

	// front and back
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .04f, .067f, .05f, 1, 0, 0, -90, 64.7f, -10, -9.25f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 270, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .04f, .067f, .05f, 1, 0, 0, -90, 90, -10, .25f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 90, true);
}

/*
* Add all the plants to the scene.
*/
void CastleScene::SetupPlants(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectSharedPtr modelSceneObject;

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
	GTE::SceneObjectSharedPtr treeMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr treeMesh = treeMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr treeRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(treeMeshObject->GetRenderer());
	GTE::MaterialSharedPtr treeMaterial = treeRenderer->GetMaterial(0);

	// place initial tree in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0015f, .0015f, .0015f, false);
	modelSceneObject->GetTransform().Rotate(.8f, 0, .2f, -6, false);
	modelSceneObject->GetTransform().Translate(55, -10.5f, 11, false);

	// reuse the tree mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .10f, .10f, .10f, 1, 0, 0, -85, 57, -10, 24, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	//modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .15f, .15f, .20f, 1, 0, 0, -94, 61, -9, -15, true, true, true);
	//sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .20f, .20f, .30f, 1, 0, 0, -93, 80, -9, -15, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .17f, .17f, .20f, 1, 0, 0, -85, 85, -9.5f, -13, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .22f, .22f, .38f, 1, 0, 0, -90, 115, -9.5f, 15, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .19f, .19f, .28f, 1, 0, 0, -96, 105, -9.5f, 8, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .18f, .18f, .20f, 1, 0, 0, -87, 95, -10, 32, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
}

/*
* Add miscellaneous elements to the scene.
*/
void CastleScene::SetupExtra(GTE::AssetImporter& importer)
{
	// misc. reference variables
	GTE::SceneObjectSharedPtr modelSceneObject;

	//========================================================
	//
	// Fences
	//
	//========================================================

	// load fence model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrier01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load barrier model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract fence mesh & material
	GTE::SceneObjectSharedPtr fenceMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr fenceMesh = fenceMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr fenceRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(fenceMeshObject->GetRenderer());
	GTE::MaterialSharedPtr fenceMaterial = fenceRenderer->GetMaterial(0);

	// place initial fence in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.6f, .6f, .6f, false);
	modelSceneObject->GetTransform().Translate(60, -10, 22, false);

	/** re-use the fence mesh & material for multiple instances  **/

	// fence on right side of castle
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 47, -10, 14.5f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -70, true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 52, -10, 20, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -25, true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 69.5, -10, 21, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 15, true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 79.2f, -10, 19.3f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 5, true);

	// fence on left side of castle
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 59.9f, -10, -33.3f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -135, true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 68.2f, -10, -38, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -160, true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f, .6f, .6f, 1, 0, 0, -90, 78, -10, -40, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, -175, true);


	//========================================================
	//
	// Barrels
	//
	//========================================================

	// load barrel model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrel01.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load barrel model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract barrel mesh and material
	GTE::SceneObjectSharedPtr barrelMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DSharedPtr barrelMesh = barrelMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr barrelRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(barrelMeshObject->GetRenderer());
	GTE::MaterialSharedPtr barrelMaterial = barrelRenderer->GetMaterial(0);

	// place initial barrel in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.8f, .8f, .8f, false);
	modelSceneObject->GetTransform().Translate(82, -10, 14.5f, false);

	// re-use the barrel mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .8f, .8f, .8f, 1, 0, 0, -90, 70, -10, 14.5f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9f, .9f, .9f, 0, 1, 0, 90, 92, -8.3f, 1.5f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9f, .9f, .9f, 0, 1, 0, 90, 92, -8.3f, -1.8f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9f, .9f, .9f, 0, 1, 0, 90, 92, -5.3f, -.15f, true, true, true);
	sceneRoot->AddChild(modelSceneObject);


	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	//========================================================
	//
	// Normal mapped castle floor
	//
	//========================================================

	// create instance of SceneObject to hold the floor mesh and its renderer
	GTE::SceneObjectSharedPtr cubeSceneObject = objectManager->CreateSceneObject();
	cubeSceneObject->SetStatic(true);
	sceneRoot->AddChild(cubeSceneObject);

	// load texture for the floor
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	GTE::TextureSharedPtr texture = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/color.png", texAttributes);
	GTE::TextureSharedPtr normalmap = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/normal.png", texAttributes);


	// create the floor's material using the "basic_normal" built-in shader
	GTE::ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic_normal", basicShaderSource);
	GTE::MaterialSharedPtr material = objectManager->CreateMaterial(std::string("BasicMaterial"), basicShaderSource);
	material->SetTexture(texture, "TEXTURE0");
	material->SetTexture(normalmap, "NORMALMAP");
	material->SetUniform1f(3.4f, "USCALE");
	material->SetUniform1f(3.4f, "VSCALE");
	material->SetUniform1f(1.0f, "SPECULAR_FACTOR");

	// set the floor mesh attributes
	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::UVTexture0);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::VertexColor);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Tangent);

	// create the floor mesh
	GTE::Mesh3DSharedPtr cubeMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Mesh3DFilterSharedPtr cubeMeshfilter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(cubeMeshfilter);
	cubeMeshfilter->SetMesh3D(cubeMesh);
	cubeMeshfilter->SetCastShadows(false);
	cubeMeshfilter->SetReceiveShadows(true);

	// create the floor mesh's renderer
	GTE::Mesh3DRendererSharedPtr renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::Mesh3DRenderer, GTE::Renderer>(renderer));

	// scale the floor and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(12, 1.5f, 10, false);
	cubeSceneObject->GetTransform().Translate(78, -11.4f, -4, false);
}

/*
* Set up the lights that belong to this scene.
*/
void CastleScene::SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject)
{
	GTE::SceneObjectSharedPtr sceneObject;

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	// create material for light meshes
	GTE::ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	GTE::MaterialSharedPtr selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	selflitMaterial->SetColor(GTE::Color4(1, 1, 1, 1), "SELFCOLOR");

	//========================================================
	//
	// Castle lights
	//
	//========================================================

	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);

	// create mesh & material for castle lantern
	GTE::Mesh3DSharedPtr lanternLightMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Color4 lanternLightColor(1, .66f, .231f, 1);
	GTE::Color4 lanternLightMeshColor(1, .95f, .5f, 1);
	GTE::MaterialSharedPtr lanterLightMeshMaterial = objectManager->CreateMaterial("LanternLightMeshMaterial", selfLitShaderSource);
	lanterLightMeshMaterial->SetColor(lanternLightMeshColor, "SELFCOLOR");
	lanterLightMeshMaterial->SetUseLighting(false);

	// create castle center lantern
	GTE::SceneObjectSharedPtr lanternObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lanternObject);
	//lanternObject->SetStatic(true);
	GTE::LightSharedPtr lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.8f);
	lanternLight->SetRange(25);
	lanternLight->SetColor(lanternLightColor);
	GTE::IntMask mergedMask = lanternLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	lanternLight->SetCullingMask(mergedMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(GTE::LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2f, .2f, .2f, true);
	//lanternObject->GetTransform().Translate(75f, -6, -5, false);
	lanternObject->GetTransform().Translate(72.0f, -1, -8, false);
	GTE::Mesh3DFilterSharedPtr filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	GTE::Mesh3DRendererSharedPtr lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::Mesh3DRenderer, GTE::Renderer>(lanterLightRenderer));
	pointLights.push_back(lanternObject); 
	movingLightA = lanternObject;

	// create castle side lantern
	lanternObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lanternObject);
	lanternObject->SetStatic(true);
	lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.5f);
	lanternLight->SetRange(20);
	lanternLight->SetColor(lanternLightColor);
	mergedMask = lanternLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	lanternLight->SetCullingMask(mergedMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(GTE::LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2f, .2f, .2f, true);
	lanternObject->GetTransform().Translate(77.4f, -5, 14.7f, false);
	filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetRenderer(GTE::DynamicCastEngineObject<GTE::Mesh3DRenderer, GTE::Renderer>(lanterLightRenderer));
	pointLights.push_back(lanternObject);
}

void CastleScene::SetupCampfire(GTE::AssetImporter& importer, GTE::SceneObjectSharedPtr playerObject)
{
	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();


	//================================
	// Setup flame particle system
	//================================

	GTE::SceneObjectRef fireParentObject = objectManager->CreateSceneObject();
	ASSERT(fireParentObject.IsValid(), "Unable to create flame particle system parent object!\n");
	sceneRoot->AddChild(fireParentObject);

	GTE::SceneObjectRef flameSystemObject = objectManager->CreateSceneObject();
	ASSERT(flameSystemObject.IsValid(), "Unable to create flame particle system object!\n");
	flameSystemObject->GetTransform().Translate(0, 1, 0, true);
	fireParentObject->AddChild(flameSystemObject);

	// create material for rendering flame particles
	std::string shaderName = "particles_unlit";
	std::string materialName = "ParticlesUnlit";
	GTE::MaterialRef flameMaterial = GTE::ParticleSystem::CreateMaterial(shaderName, materialName);
	ASSERT(flameMaterial.IsValid(), "Unable to create flame material!\n");

	// load texture for the flame's atlas
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 2;
	GTE::TextureRef flameTexture = objectManager->CreateTexture("resources/textures/particles/fireloop3.jpg", texAttributes);
	ASSERT(flameTexture.IsValid(), "Unable to load flame texture!\n");

	GTE::AtlasRef flameAtlas = objectManager->CreateGridAtlas(flameTexture, 0.0f, 1.0f, 1.0f, 0.0f, 8, 8, false, true);
	ASSERT(flameAtlas.IsValid(), "Unable to create flame atlas!\n");

	//GTE::ParticleSystemRef flameSystem = objectManager->CreateParticleSystem(flameMaterial, flameAtlas, false, 3.0f, 3.0f, 0.0f);
	GTE::ParticleSystemRef flameSystem = objectManager->CreateParticleSystem(flameMaterial, flameAtlas, false, 3.5f, 3.0f, 0.0f);
	ASSERT(flameSystem.IsValid(), "Unable to create flame particle system!\n");
	flameSystemObject->SetParticleSystem(flameSystem);
	flameSystem->SetPremultiplyAlpha(true);

	GTE::RandomModifier<GTE::Point3> flamePositionModifier(GTE::Point3(0.0f, 0.0f, 0.0f), GTE::Point3(0.0f, 0.0f, 0.0f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::RandomModifier<GTE::Vector3> flameVelocityModifier(GTE::Vector3(0.0f, 2.3f, 0.0f), GTE::Vector3(0.9f, 0.4f, 0.9f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::EvenIntervalIndexModifier flameAtlasModifier(64);

	GTE::FrameSetModifier<GTE::Vector2> flameSizeModifier;
	flameSizeModifier.AddFrame(0.0f, GTE::Vector2(2.0f, 2.5f));
	flameSizeModifier.AddFrame(1.0f, GTE::Vector2(2.0f, 2.5f));

	GTE::FrameSetModifier<GTE::Real> flameAlphaModifier;
	flameAlphaModifier.AddFrame(0.0f, 0.0f);
	flameAlphaModifier.AddFrame(0.2f, 0.3f);
	flameAlphaModifier.AddFrame(1.2f, 1.0f);
	flameAlphaModifier.AddFrame(2.0f, 1.0f);
	flameAlphaModifier.AddFrame(3.0f, 0.0f);

	GTE::FrameSetModifier<GTE::Color4> flameColorModifier;
	flameColorModifier.AddFrame(0.0f, GTE::Color4(1.0f, 1.0f, 1.0f, 1.0f));
	flameColorModifier.AddFrame(0.2f, GTE::Color4(1.0f, 1.0f, 1.0f, 1.0f));
	flameColorModifier.AddFrame(3.0f, GTE::Color4(1.0f, 1.0f, 1.0f, 1.0f));

	flameSystem->BindPositionModifier(flamePositionModifier);
	flameSystem->BindVelocityModifier(flameVelocityModifier);
	flameSystem->BindSizeModifier(flameSizeModifier);
	flameSystem->BindAlphaModifier(flameAlphaModifier);
	flameSystem->BindColorModifier(flameColorModifier);
	flameSystem->BindAtlasModifier(flameAtlasModifier);


	//================================
	// Setup ember particle system
	//================================

	GTE::SceneObjectRef emberSystemObject = objectManager->CreateSceneObject();
	ASSERT(emberSystemObject.IsValid(), "Unable to create ember particle system object!\n");
	fireParentObject->AddChild(emberSystemObject);

	// create material for rendering ember particles
	shaderName = "particles_unlit";
	materialName = "ParticlesUnlit";
	GTE::MaterialRef emberMaterial = GTE::ParticleSystem::CreateMaterial(shaderName, materialName);
	ASSERT(emberMaterial.IsValid(), "Unable to create ember material!\n");

	// load texture for the ember particle
	GTE::TextureRef emberTexture = objectManager->CreateTexture("resources/textures/particles/puff.png", texAttributes);
	ASSERT(emberTexture.IsValid(), "Unable to load ember texture!\n");

	GTE::AtlasRef emberAtlas = objectManager->CreateAtlas(emberTexture, true);
	ASSERT(emberAtlas.IsValid(), "Unable to create ember atlas!\n");

	GTE::ParticleSystemRef emberSystem = objectManager->CreateParticleSystem(emberMaterial, emberAtlas, false, 9.0f, 3.0f, 0.0f);
	ASSERT(emberSystem.IsValid(), "Unable to create ember particle system!\n");
	emberSystemObject->SetParticleSystem(emberSystem);
	emberSystem->SetPremultiplyAlpha(true);

	GTE::RandomModifier<GTE::Point3> emberPositionModifier(GTE::Point3(0.0f, 2.0f, 0.0f), GTE::Point3(0.4f, 0.0f, 0.4f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::RandomModifier<GTE::Vector3> emberVelocityModifier(GTE::Vector3(0.0f, 5.0f, 0.0f), GTE::Vector3(3.0f, 5.0f, 3.0f), GTE::ParticleRangeType::Sphere, true, true);
	GTE::RandomModifier<GTE::Vector3> emberAccelerationModifier(GTE::Vector3(0.0f, 3.0f, 0.0f), GTE::Vector3(75.0f, 65.0f, 75.0f), GTE::ParticleRangeType::Sphere, true, false);
	GTE::EvenIntervalIndexModifier emberAtlasModifier(1);
	GTE::RandomModifier<GTE::Vector2> emberSizeModifier(GTE::Vector2(0.05f, 0.05f), GTE::Vector2(0.01f, 0.01f), GTE::ParticleRangeType::Sphere, false, true);

	GTE::FrameSetModifier<GTE::Real> emberAlphaModifier;
	emberAlphaModifier.AddFrame(0.0f, 0.0f);
	emberAlphaModifier.AddFrame(0.2f, 1.0f);
	emberAlphaModifier.AddFrame(1.2f, 1.0f);
	emberAlphaModifier.AddFrame(2.0f, 1.0f);
	emberAlphaModifier.AddFrame(3.0f, 0.0f);

	GTE::FrameSetModifier<GTE::Color4> emberColorModifier;
	emberColorModifier.AddFrame(0.0f, GTE::Color4(1.3f, 1.3f, 0.0f, 1.0f));
	emberColorModifier.AddFrame(0.2f, GTE::Color4(0.75f, 0.4f, 0.4f, 1.0f));
	emberColorModifier.AddFrame(3.0f, GTE::Color4(0.6f, 0.6f, 0.6f, 0.0f));

	emberSystem->BindPositionModifier(emberPositionModifier);
	emberSystem->BindVelocityModifier(emberVelocityModifier);
	emberSystem->BindSizeModifier(emberSizeModifier);
	emberSystem->BindAlphaModifier(emberAlphaModifier);
	emberSystem->BindColorModifier(emberColorModifier);
	emberSystem->BindAtlasModifier(emberAtlasModifier);
	emberSystem->BindAccelerationModifier(emberAccelerationModifier);

	//================================
	// Setup smoke particle system
	//================================

	GTE::SceneObjectRef smokeSystemObject = objectManager->CreateSceneObject();
	ASSERT(smokeSystemObject.IsValid(), "Unable to create smoke particle system object!\n");
	fireParentObject->AddChild(smokeSystemObject);

	// create material for rendering smoke particles
	shaderName = "particles_lit";
	materialName = "ParticlesLit";
	GTE::MaterialRef smokeMaterial = GTE::ParticleSystem::CreateMaterial(shaderName, materialName);
	ASSERT(smokeMaterial.IsValid(), "Unable to create smoke material!\n");
	smokeMaterial->SetBlendingMode(GTE::RenderState::BlendingMode::Custom);
	smokeMaterial->SetSourceBlendingMethod(GTE::RenderState::BlendingMethod::SrcAlpha);
	smokeMaterial->SetDestBlendingMethod(GTE::RenderState::BlendingMethod::OneMinusSrcAlpha);
	smokeMaterial->SetUseLighting(true);
	smokeMaterial->SetRenderQueue((GTE::UInt32)GTE::RenderQueueType::Transparent + 1);

	// load texture for the smoke particle
	GTE::TextureRef smokeTexture = objectManager->CreateTexture("resources/textures/particles/smokeparticle.png", texAttributes);
	ASSERT(smokeTexture.IsValid(), "Unable to load smoke texture!\n");

	GTE::AtlasRef smokeAtlas = objectManager->CreateAtlas(smokeTexture, true);
	ASSERT(smokeAtlas.IsValid(), "Unable to create smoke atlas!\n");

	GTE::ParticleSystemRef smokeSystem = objectManager->CreateParticleSystem(smokeMaterial, smokeAtlas, false, 150.0f, 4.0f, 0.0f);
	ASSERT(smokeSystem.IsValid(), "Unable to create smoke particle system!\n");
	smokeSystemObject->SetParticleSystem(smokeSystem);
	smokeSystem->SetPremultiplyAlpha(false);
	smokeSystem->SetZSort(true);

	GTE::RandomModifier<GTE::Point3> smokePositionModifier(GTE::Point3(0.0f, 0.0f, 0.0f), GTE::Point3(0.5f, 0.0f, 0.5f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::RandomModifier<GTE::Vector3> smokeVelocityModifier(GTE::Vector3(0.0f, 9.0f, 0.0f), GTE::Vector3(0.5f, 2.6f, 2.5f), GTE::ParticleRangeType::Sphere, false, true);
	GTE::RandomModifier<GTE::Vector3> smokeAccelerationModifier(GTE::Vector3(0.0f, -1.8f, 0.0f), GTE::Vector3(3.5f, 2.0f, 3.5f), GTE::ParticleRangeType::Cube, false, true);
	GTE::EvenIntervalIndexModifier smokeAtlasModifier(1);

	GTE::FrameSetModifier<GTE::Vector2> smokeSizeModifier;
	smokeSizeModifier.AddFrame(0.0f, GTE::Vector2(1.0f, 1.0f));
	smokeSizeModifier.AddFrame(4.0f, GTE::Vector2(5.0f, 5.0f));

	GTE::FrameSetModifier<GTE::Real> smokeAlphaModifier;
	smokeAlphaModifier.AddFrame(0.0f, 0.0f);
	smokeAlphaModifier.AddFrame(1.0f, 0.16f);
	smokeAlphaModifier.AddFrame(2.0f, 0.32f);
	smokeAlphaModifier.AddFrame(3.0f, 0.16f);
	smokeAlphaModifier.AddFrame(4.0f, 0.0f);

	GTE::FrameSetModifier<GTE::Color4> smokeColorModifier;
	smokeColorModifier.AddFrame(0.0f, GTE::Color4(0.4f, 0.4f, 0.4f, 1.0f));
	smokeColorModifier.AddFrame(1.5f, GTE::Color4(0.4f, 0.4f, 0.4f, 1.0f));
	smokeColorModifier.AddFrame(4.0f, GTE::Color4(0.6f, 0.6f, 0.6f, 1.0f));

	smokeSystem->BindPositionModifier(smokePositionModifier);
	smokeSystem->BindVelocityModifier(smokeVelocityModifier);
	smokeSystem->BindSizeModifier(smokeSizeModifier);
	smokeSystem->BindAlphaModifier(smokeAlphaModifier);
	smokeSystem->BindColorModifier(smokeColorModifier);
	smokeSystem->BindAtlasModifier(smokeAtlasModifier);
	smokeSystem->BindAccelerationModifier(smokeAccelerationModifier);

	GTE::LayerManager& layerManager = objectManager->GetLayerManager();
	GTE::Int32 smokeLayerIndex = layerManager.AddLayer(SmokeLayer);
	GTE::IntMask smokeLayerMask = layerManager.GetLayerMask(smokeLayerIndex);
	smokeSystem->GetMeshSceneObject()->SetLayerMask(smokeLayerMask);

	//================================
	// Setup campfire light
	//================================

	campFireLightObject = objectManager->CreateSceneObject();
	fireParentObject->AddChild(campFireLightObject);
	campFireLightObject->SetStatic(true);
	campFireLight = objectManager->CreateLight();
	campFireLight->SetIntensity(1.8f);
	campFireLight->SetRange(20);
	campFireLight->SetColor(1.0f, 0.6f, 0.0f, 1.0f);
	GTE::IntMask mergedMask = campFireLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, smokeLayerMask);
	campFireLight->SetCullingMask(mergedMask);
	campFireLight->SetShadowsEnabled(true);
	campFireLight->SetType(GTE::LightType::Point);
	campFireLightObject->SetLight(campFireLight);
	pointLights.push_back(campFireLightObject);
	campFireLightLocalOffset.Set(0, 5, 0);
	campFireLightObject->GetTransform().Translate(campFireLightLocalOffset, true);

	//================================
	// Load campfire model
	//================================

	GTE::SceneObjectRef campfireSceneObject = importer.LoadModelDirect("resources/models/toonlevel/campfire/campfire01.fbx");
	ASSERT(campfireSceneObject.IsValid(), "Could not load campfire model!\n");
	campfireSceneObject->GetTransform().Scale(0.8f, 0.6f, 0.8f, true);
	fireParentObject->AddChild(campfireSceneObject);
	campfireSceneObject->SetActive(true);
	GameUtil::SetAllObjectsStatic(campfireSceneObject);
	GameUtil::SetAllObjectsCastShadows(campfireSceneObject, true);
	GameUtil::SetAllObjectsReceiveShadows(campfireSceneObject, false);

	GTE::SceneObjectSharedPtr campfireMeshObject = GameUtil::FindFirstSceneObjectWithMesh(campfireSceneObject);
	GTE::Mesh3DSharedPtr campFireMesh = campfireMeshObject->GetMesh3D();
	GTE::Mesh3DRendererSharedPtr campfireRenderer = GTE::DynamicCastEngineObject<GTE::Renderer, GTE::Mesh3DRenderer>(campfireMeshObject->GetRenderer());
	GTE::MaterialSharedPtr campfireMaterial = campfireRenderer->GetMaterial(0);
	campfireMaterial->SetFaceCulling(GTE::RenderState::FaceCulling::None);

	// move campfire to its final location
	fireParentObject->GetTransform().Translate(66.0f, -10.0f, -28.0f, false);
}

void CastleScene::FlickerCampFireLightLight()
{
	GTE::Real time = GTE::Time::GetTime();
	GTE::Real elapsedTime = time - lastFlickerTime;

	if(elapsedTime > 1.0f / 30.0f)
	{
		lastFlickerTime = time;

		GTE::Real intensityAdjuster = (GTE::GTEMath::Random() - 0.5f);
		GTE::Vector3 positionAdjuster(GTE::GTEMath::Random() - 0.5f, GTE::GTEMath::Random() - 0.5f, GTE::GTEMath::Random() - 0.5f);

		GTE::Real deltaTime = GTE::Time::GetDeltaTime();
		intensityAdjuster *= deltaTime * 220.0f;
		positionAdjuster.Scale(deltaTime * 50.0f);

		GTE::Real diff = (intensityAdjuster - lastCampFireLightIntenistyAdjuster) * 0.2f;
		intensityAdjuster = lastCampFireLightIntenistyAdjuster + diff;

		GTE::Real intensity = 3.0f;
		intensity += intensityAdjuster * 1.5f;
		campFireLight->SetIntensity(intensity);

		campFireLightObject->GetTransform().SetIdentity();
		campFireLightObject->GetTransform().Translate(campFireLightLocalOffset, true);

		positionAdjuster.Add(lastCampFireLightPositionAdjuster);
		positionAdjuster.Scale(0.5f);

		campFireLightObject->GetTransform().Translate(positionAdjuster, true);

		lastCampFireLightPositionAdjuster = positionAdjuster;
		lastCampFireLightIntenistyAdjuster = intensityAdjuster;
	}
}

std::vector<GTE::SceneObjectSharedPtr>& CastleScene::GetPointLights()
{
	return pointLights;
}
