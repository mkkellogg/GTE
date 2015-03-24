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
SceneObjectRef CastleScene::GetSceneRoot()
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

}

/*
 * Set up all elements of the scene using [importer] to load any assets from disk.
 *
 * [ambientLightObject] - Global scene object that contains the global ambient light.
 * [directLightObject] - Global scene object that contains the global directional light.
 * [playerObject] - Scene object that contains the player mesh & renderer.
 */
void CastleScene::Setup(AssetImporter& importer, SceneObjectRef ambientLightObject, SceneObjectRef directionalLightObject, SceneObjectRef playerObject)
{
	importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);

	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	sceneRoot = objectManager->CreateSceneObject();
	ASSERT_RTRN(sceneRoot.IsValid(), "Could not create scene root for castle scene!\n");

	sceneRoot->GetTransform().Translate(50,0,-15, false);

	SetupTerrain(importer);
	SetupStructures(importer);
	SetupPlants(importer);
	SetupExtra(importer);
	SetupLights(importer,playerObject);

	sceneRoot->GetTransform().Rotate(0,1,0, 155, true);
	sceneRoot->GetTransform().Translate(-44.3, 0, 52, false);


	this->directionalLightObject = directionalLightObject;
}

/*
* Set up the "land" elements in the scene.
*/
void CastleScene::SetupTerrain(AssetImporter& importer)
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
	modelSceneObject->GetTransform().Scale(.07,.07,.07, false);
	modelSceneObject->GetTransform().Translate(80,-10,-10,false);
}

/*
* Set up all the man-made structures, buildings, etc. in the scene.
*/
void CastleScene::SetupStructures(AssetImporter& importer)
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
	modelSceneObject->GetTransform().Scale(.04,.03,.04, false);
	modelSceneObject->GetTransform().Translate(65,-10,-15,false);

	// re-use the castle tower mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 89,-10,-15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 65,-10,6, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 89,-10,6, true,true,true);
	sceneRoot->AddChild(modelSceneObject);

	// load & place castle entrance arch-way left side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Left_02.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.04,.04, false);
	modelSceneObject->GetTransform().Rotate(0,1,0,90,false);
	modelSceneObject->GetTransform().Translate(65,-10,-8.8,false);

	// load and place castle entrance arch-way right side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Right_02.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load wall model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.04,.04, false);
	modelSceneObject->GetTransform().Rotate(0,1,0, 90,false);
	modelSceneObject->GetTransform().Translate(65,-10,0,false);

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
	modelSceneObject->GetTransform().Scale(.06,.05,.04, false);
	modelSceneObject->GetTransform().Translate(70,-10,6.5,false);

	// re-use the castle wall mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 78,-10,6.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 86,-10,6.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 70,-10,-15.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 78,-10, -15.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 86,-10, -15.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.067,.05, 1,0,0, -90, 90,-10, -9.25, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);
	modelSceneObject = GameUtil::AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.067,.05, 1,0,0, -90, 90,-10, .25, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);
}

/*
* Add all the plants to the scene.
*/
void CastleScene::SetupPlants(AssetImporter& importer)
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
void CastleScene::SetupExtra(AssetImporter& importer)
{
	// misc. reference variables
	SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Fences
	//
	//========================================================

	// load fence model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrier01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load barrier model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract fence mesh & material
	SceneObjectRef fenceMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef fenceMesh = fenceMeshObject->GetMesh3D();
	Mesh3DRendererRef fenceRenderer = fenceMeshObject->GetMesh3DRenderer();
	MaterialRef fenceMaterial = fenceRenderer->GetMaterial(0);

	// place initial fence in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.6,.6,.6, false);
	modelSceneObject->GetTransform().Translate(60,-10,22,false);

	/** re-use the fence mesh & material for multiple instances  **/

	// fence on right side of castle
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 47,-10,14.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-70,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 52,-10,20, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-25,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 69.5,-10,21, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,15,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 79.2,-10,19.3, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,5,true);

	// fence on left side of castle
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 51,-10,-16, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-120,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 55,-10,-25, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-110,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 59.9,-10,-33.3, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-135,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 68.2,-10,-38, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-160,true);
	modelSceneObject = GameUtil::AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 78,-10,-40, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject->GetTransform().Rotate(0,0,1,-175,true);


	//========================================================
	//
	// Barrels
	//
	//========================================================

	// load barrel model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrel01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load barrel model!\n");
	sceneRoot->AddChild(modelSceneObject);
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// extract barrel mesh and material
	SceneObjectRef barrelMeshObject = GameUtil::FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef barrelMesh = barrelMeshObject->GetMesh3D();
	Mesh3DRendererRef barrelRenderer = barrelMeshObject->GetMesh3DRenderer();
	MaterialRef barrelMaterial = barrelRenderer->GetMaterial(0);

	// place initial barrel in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.8,.8,.8, false);
	modelSceneObject->GetTransform().Translate(82,-10,10.5, false);

	// re-use the barrel mesh & material for multiple instances
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .8,.8,.8, 1,0,0, -90, 70,-10,10.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-8.3,1.5, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-8.3,-1.8, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
	modelSceneObject = GameUtil::AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-5.3,-.15, true,true,true);
	sceneRoot->AddChild(modelSceneObject);
}

/*
* Set up the lights that belong to thsi scene.
*/
void CastleScene::SetupLights(AssetImporter& importer, SceneObjectRef playerObject)
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
	// Castle lights
	//
	//========================================================

	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	// create mesh & material for castle lantern
	Mesh3DRef lanternLightMesh = EngineUtility::CreateCubeMesh(meshAttributes);
	Color4 lanternLightColor(1,.66,.231,1);
	Color4 lanternLightMeshColor(1,.95,.5,1);
	MaterialRef lanterLightMeshMaterial = objectManager->CreateMaterial("LanternLightMeshMaterial", selfLitShaderSource);
	lanterLightMeshMaterial->SetColor(lanternLightMeshColor, "SELFCOLOR");
	lanterLightMeshMaterial->SetSelfLit(true);

	// create castle center lantern
	SceneObjectRef lanternObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lanternObject);
	lanternObject->SetStatic(true);
	LightRef lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.8);
	lanternLight->SetRange(25);
	lanternLight->SetColor(lanternLightColor);
	IntMask mergedMask = lanternLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	lanternLight->SetCullingMask(mergedMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2,.2,.2, true);
	lanternObject->GetTransform().Translate(77.2, -4, -5, false);
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	Mesh3DRendererRef lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
	pointLights.push_back(lanternObject);

	// create castle side lantern
	lanternObject = objectManager->CreateSceneObject();
	sceneRoot->AddChild(lanternObject);
	lanternObject->SetStatic(true);
	lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.5);
	lanternLight->SetRange(20);
	lanternLight->SetColor(lanternLightColor);
	mergedMask = lanternLight->GetCullingMask();
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(mergedMask, playerObject->GetLayerMask());
	lanternLight->SetCullingMask(mergedMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2,.2,.2, true);
	lanternObject->GetTransform().Translate(77.4, -5, 8.7, false);
	filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
	pointLights.push_back(lanternObject);
}

std::vector<SceneObjectRef>& CastleScene::GetPointLights()
{
	return pointLights;
}
