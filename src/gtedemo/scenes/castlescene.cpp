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
CastleScene::CastleScene() : Scene()
{

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
GTE::SceneObjectRef CastleScene::GetSceneRoot()
{
	return sceneRoot;
}

/*
 * Called whenever this scene is activated.
 */
void CastleScene::OnActivate()
{
	//directionalLightObject->GetLight()->SetDirection(.8,-1.7,2);
	//Engine::Instance()->GetRenderManager()->ClearCaches();
}


/*
 * Update() is called once per frame from the Game() instance.
 */
void CastleScene::Update()
{
	GTE::Point3 lightRotatePoint(-14,1,17);
	// rotate the point light around [lightRotatePoint]
	movingLightA->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z, 0, 1, 0, 60 * GTE::Time::GetDeltaTime(), false);
}

/*
 * Set up all elements of the scene using [importer] to load any assets from disk.
 *
 * [ambientLightObject] - Global scene object that contains the global ambient light.
 * [directLightObject] - Global scene object that contains the global directional light.
 * [playerObject] - Scene object that contains the player mesh & renderer.
 */
void CastleScene::Setup(GTE::AssetImporter& importer, GTE::SceneObjectRef ambientLightObject, GTE::SceneObjectRef directionalLightObject, GTE::SceneObjectRef playerObject)
{
	importer.SetBoolProperty(GTE::AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT(sceneRoot.IsValid(), "Could not create scene root for castle scene!\n");

	sceneRoot->GetTransform().Translate(50,0,-15, false);

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupPlants(importer);
	SetupExtra(importer);
	SetupLights(importer,playerObject);

	sceneRoot->GetTransform().Rotate(0,1,0, 155, true);
	sceneRoot->GetTransform().Translate(-44.3f, 0, 52, false);

	this->directionalLightObject = directionalLightObject;
}

/*
* Set up the "land" elements in the scene.
*/
void CastleScene::SetupTerrain(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

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
	modelSceneObject->GetTransform().Scale(.07f,.07f,.07f, false);
	modelSceneObject->GetTransform().Translate(80,-10,-10,false);
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void CastleScene::SetupStructures(GTE::AssetImporter& importer)
{
	// multi-use reference
	GTE::SceneObjectRef modelSceneObject;

	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();



	// load texture for the castle
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	GTE::TextureRef texture = objectManager->CreateTexture("resources/models/toonlevel/castle/Castle01.png", texAttributes);
	GTE::TextureRef normalmap = objectManager->CreateTexture("resources/models/toonlevel/castle/Castle01Normals.png", texAttributes);

	// create the castle's material using the "basic_normal" built-in shader
	GTE::ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic_normal", basicShaderSource);
	GTE::MaterialRef castleNormalMaterial = objectManager->CreateMaterial(std::string("CastleNormalMat"), basicShaderSource);
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
	GTE::SceneObjectRef tower2MeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef tower2Mesh = tower2MeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef towerRenderer = tower2MeshObject->GetMesh3DRenderer();
	GTE::Mesh3DFilterRef towerMeshFilter = tower2MeshObject->GetMesh3DFilter();
	towerMeshFilter->SetUseBackSetShadowVolume(true);
	
	tower2Mesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	tower2Mesh->GetSubMesh(0)->SetCalculateNormals(true);
	tower2Mesh->GetSubMesh(0)->SetCalculateTangents(true);
	tower2Mesh->Update();
	towerRenderer->SetMaterial(0, castleNormalMaterial);

	// place initial castle tower in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04f,.03f,.04f, false);
	modelSceneObject->GetTransform().Translate(65,-10,-15,false);

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
	modelSceneObject->GetTransform().Scale(.04f,.04f,.04f, false);
	modelSceneObject->GetTransform().Rotate(0,1,0,90,false);
	modelSceneObject->GetTransform().Translate(65,-10,-8.8f,false);

	// extract mesh & material from left archway model
	GTE::SceneObjectRef archwayMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef archwayMesh = archwayMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef archwayRenderer = archwayMeshObject->GetMesh3DRenderer();
	GTE::Mesh3DFilterRef archwayMeshFilter = archwayMeshObject->GetMesh3DFilter();
	archwayMeshFilter->SetUseBackSetShadowVolume(false);

	// prep left archway mesh & material for normal mapped rendering
	archwayMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	archwayMesh->GetSubMesh(0)->SetCalculateNormals(true);
	archwayMesh->GetSubMesh(0)->SetCalculateTangents(true);
	archwayMesh->Update();
	archwayRenderer->SetMaterial(0, castleNormalMaterial);
	modelSceneObject->SetActive(false);
	
	// load and place castle entrance arch-way right side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Right_02.fbx");
	ASSERT(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04f,.04f,.04f, false);
	modelSceneObject->GetTransform().Rotate(0,1,0, 90,false);
	modelSceneObject->GetTransform().Translate(65,-10,0,false);
	
	// extract mesh & material from right archway model
	archwayMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	archwayMesh = archwayMeshObject->GetMesh3D();
	archwayRenderer = archwayMeshObject->GetMesh3DRenderer();
	archwayMeshFilter = archwayMeshObject->GetMesh3DFilter();
	archwayMeshFilter->SetUseBackSetShadowVolume(false);

	// prep right archway mesh & material for normal mapped rendering
	archwayMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	archwayMesh->GetSubMesh(0)->SetCalculateNormals(true);
	archwayMesh->GetSubMesh(0)->SetCalculateTangents(true);
	archwayMesh->Update();
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
	GTE::SceneObjectRef wallBlockMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef wallBlockRenderer = wallBlockMeshObject->GetMesh3DRenderer();
	GTE::Mesh3DFilterRef wallMeshFilter = wallBlockMeshObject->GetMesh3DFilter();
	wallMeshFilter->SetUseBackSetShadowVolume(false);
	
	wallBlockMesh->GetSubMesh(0)->SetNormalsSmoothingThreshold(25);
	wallBlockMesh->GetSubMesh(0)->SetCalculateNormals(true);
	wallBlockMesh->GetSubMesh(0)->SetCalculateTangents(true);
	wallBlockMesh->Update();
	wallBlockRenderer->SetMaterial(0, castleNormalMaterial);
		
	// place initial castle wall in scene (on right side)
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.06f,.05f,.04f, false);
	modelSceneObject->GetTransform().Translate(70,-10,6.5f,false);
	modelSceneObject->GetTransform().Rotate(0, 1, 0, 180, true);

	// right side
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 78, -10, 6.5f, true, true, true,false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 86, -10, 6.5f, true, true, true,false);
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
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 70, -10, -15.5f, true, true, true,false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 78, -10, -15.5f, true, true, true,false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .06f, .04f, .05f, 1, 0, 0, -90, 86, -10, -15.5f, true, true, true,false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0, 0, 1, 180, true);

	// front and back
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .04f, .067f, .05f, 1, 0, 0, -90, 64.7f, -10, -9.25f, true, true, true,false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, castleNormalMaterial, .04f, .067f, .05f, 1, 0, 0, -90, 90, -10, .25f, true, true, true, false);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);
}

/*
* Add all the plants to the scene.
*/
void CastleScene::SetupPlants(GTE::AssetImporter& importer)
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

	// extract tree mesh & material
	GTE::SceneObjectRef treeMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef treeMesh = treeMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef treeRenderer = treeMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef treeMaterial = treeRenderer->GetMaterial(0);

	// place initial tree in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0015f,.0015f,.0015f, false);
	modelSceneObject->GetTransform().Rotate(.8f,0,.2f, -6, false);
	modelSceneObject->GetTransform().Translate(55,-10.5f, 11,false);

	// reuse the tree mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .10f,.10f,.10f, 1,0,0, -85, 57, -10, 24, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .15f,.15f,.20f, 1,0,0, -94, 61, -9, -15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .20f,.20f,.30f, 1,0,0, -93, 80, -9, -15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .17f,.17f,.20f, 1,0,0, -85, 85, -9.5f, -13, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .22f,.22f,.38f, 1,0,0, -90, 115, -9.5f, 15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .19f,.19f,.28f, 1,0,0, -96, 105, -9.5f, 8, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(treeMesh, treeMaterial, .18f,.18f,.20f, 1,0,0, -87, 95, -10, 32, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllMeshesStandardShadowVolume(modelSceneObject);
}

/*
* Add miscellaneous elements to the scene.
*/
void CastleScene::SetupExtra(GTE::AssetImporter& importer)
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
	ASSERT(modelSceneObject.IsValid(), "Could not load barrier model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract fence mesh & material
	GTE::SceneObjectRef fenceMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef fenceMesh = fenceMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef fenceRenderer = fenceMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef fenceMaterial = fenceRenderer->GetMaterial(0);

	// place initial fence in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.6f,.6f,.6f, false);
	modelSceneObject->GetTransform().Translate(60,-10,22,false);

	/** re-use the fence mesh & material for multiple instances  **/

	// fence on right side of castle
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 47,-10,14.5f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-70,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 52,-10,20, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-25,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 69.5,-10,21, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,15,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 79.2f,-10,19.3f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,5,true);

	// fence on left side of castle
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 51,-10,-16, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-120,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 55,-10,-25, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-110,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 59.9f,-10,-33.3f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-135,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 68.2f,-10,-38, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-160,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6f,.6f,.6f, 1,0,0, -90, 78,-10,-40, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-175,true);


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
	GTE::SceneObjectRef barrelMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	GTE::Mesh3DRef barrelMesh = barrelMeshObject->GetMesh3D();
	GTE::Mesh3DRendererRef barrelRenderer = barrelMeshObject->GetMesh3DRenderer();
	GTE::MaterialRef barrelMaterial = barrelRenderer->GetMaterial(0);

	// place initial barrel in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.8f,.8f,.8f, false);
	modelSceneObject->GetTransform().Translate(82,-10,14.5f, false);

	// re-use the barrel mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .8f,.8f,.8f, 1,0,0, -90, 70,-10,14.5f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9f,.9f,.9f, 0,1,0, 90, 92,-8.3f,1.5f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9f,.9f,.9f, 0,1,0, 90, 92,-8.3f,-1.8f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9f,.9f,.9f, 0,1,0, 90, 92,-5.3f,-.15f, true,true,true);
	sceneRoot->AddChild(modelSceneObject);




	// get reference to the engine's object manager
	GTE::EngineObjectManager * objectManager = GTE::Engine::Instance()->GetEngineObjectManager();

	//========================================================
	//
	// Normal mapped castle floor
	//
	//========================================================

	// create instance of SceneObject to hold the floor mesh and its renderer
	GTE::SceneObjectRef cubeSceneObject = objectManager->CreateSceneObject();
	cubeSceneObject->SetStatic(true);
	sceneRoot->AddChild(cubeSceneObject);

	// load texture for the floor
	GTE::TextureAttributes texAttributes;
	texAttributes.FilterMode = GTE::TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	GTE::TextureRef texture = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/color.png", texAttributes);
	GTE::TextureRef normalmap = objectManager->CreateTexture("resources/textures/normalmapped/bubblegrip/normal.png", texAttributes);


	// create the floor's material using the "basic_normal" built-in shader
	GTE::ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic_normal", basicShaderSource);
	GTE::MaterialRef material = objectManager->CreateMaterial(std::string("BasicMaterial"), basicShaderSource);
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
	GTE::Mesh3DRef cubeMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Mesh3DFilterRef cubeMeshfilter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(cubeMeshfilter);
	cubeMeshfilter->SetMesh3D(cubeMesh);
	cubeMeshfilter->SetCastShadows(false);
	cubeMeshfilter->SetReceiveShadows(true);

	// create the floor mesh's renderer
	GTE::Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetMesh3DRenderer(renderer);

	// scale the floor and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(12, 1.5f, 10, false);
	cubeSceneObject->GetTransform().Translate(78, -11.4f, -4, false);
}

/*
* Set up the lights that belong to thsi scene.
*/
void CastleScene::SetupLights(GTE::AssetImporter& importer, GTE::SceneObjectRef playerObject)
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
	// Castle lights
	//
	//========================================================

	GTE::StandardAttributeSet meshAttributes = GTE::StandardAttributes::CreateAttributeSet();
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Position);
	GTE::StandardAttributes::AddAttribute(&meshAttributes, GTE::StandardAttribute::Normal);

	// create mesh & material for castle lantern
	GTE::Mesh3DRef lanternLightMesh = GTE::EngineUtility::CreateCubeMesh(meshAttributes);
	GTE::Color4 lanternLightColor(1, .66f, .231f, 1);
	GTE::Color4 lanternLightMeshColor(1, .95f, .5f, 1);
	GTE::MaterialRef lanterLightMeshMaterial = objectManager->CreateMaterial("LanternLightMeshMaterial", selfLitShaderSource);
	lanterLightMeshMaterial->SetColor(lanternLightMeshColor, "SELFCOLOR");
	lanterLightMeshMaterial->SetSelfLit(true);

	// create castle center lantern
	GTE::SceneObjectRef lanternObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lanternObject);
	//lanternObject->SetStatic(true);
	GTE::LightRef lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.8f);
	lanternLight->SetRange(25);
	lanternLight->SetColor(lanternLightColor);
	GTE::IntMask mergedMask = lanternLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	lanternLight->SetCullingMask(mergedMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(GTE::LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2f,.2f,.2f, true);
	//lanternObject->GetTransform().Translate(75f, -6, -5, false);
	lanternObject->GetTransform().Translate(72.0f, -1, -8, false);
	GTE::Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	GTE::Mesh3DRendererRef lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
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
	lanternObject->GetTransform().Scale(.2f,.2f,.2f, true);
	lanternObject->GetTransform().Translate(77.4f, -5, 14.7f, false);
	filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
	pointLights.push_back(lanternObject);
}

std::vector<GTE::SceneObjectRef>& CastleScene::GetPointLights()
{
	return pointLights;
}
