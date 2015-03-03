#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <iostream>
#include <memory>
#include <functional>
#include "game.h"
#include "lavafield.h"
#include "engine.h"
#include "input/inputmanager.h"
#include "gameutil.h"
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
#include "graphics/stdattributes.h"
#include "graphics/object/submesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/animationmanager.h"
#include "graphics/animation/animationinstance.h"
#include "graphics/animation/animationplayer.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/material.h"
#include "graphics/view/camera.h"
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


const std::string Game::LavaWallLayer = "LavaWall";
const std::string Game::LavaIslandLayer = "LavaIsland";
const std::string Game::LavaIslandObjectsLayer = "LavaIslandObjects";
const std::string Game::PlayerObjectLayer = "Player";

/*
 * Default constructor -> initialize all variables.
 */
Game::Game()
{
	// initialize player movement variables
	playerHorizontalSpeed = 0;
	playerIsMoving = false;
	playerWalkSpeed = 6.0;
	playerRunSpeed = 6.0;
	playerRotateSpeed = 200;
	playerSpeedSmoothing = 10;
	playerBaseY = 0;
	playerVerticalSpeed = 0;
	playerIsGrounded = false;
	playerJumpApexReached = false;
	playerLanded = false;

	// original player forward is looking down the positive z-axis
	basePlayerForward = Vector3(0,0,1);
	// original camera forward is looking down the negative z-axis
	baseCameraForward = Vector3(0,0,-1);

	// initialize player state
	playerType = PlayerType::Warrior;
	playerState = PlayerState::Waiting;
	for(unsigned int i = 0; i < MAX_PLAYER_STATES; i++)
	{
		stateActivationTime[i] = 0;
	}

	// FPS variables
	printFPS = false;
	lastFPSRetrieveTime = 0;
	lastFPS = 0;
	lastInfoPrintTime = 0;
	displayInfoChanged = false;

	selectedLighting = SceneLighting::None;

	lavaField = NULL;

	// initialize layer masks
	lavaWallLayerMask = 0;
	lavaIslandLayerMask = 0;
	lavaIslandObjectsLayerMask = 0;
	playerObjectLayerMask = 0;
}

/*
 * Clean-up
 */
Game::~Game()
{

}

/*
 * Recursively search the scene hierarchy starting at [ref] for an
 * instance of SceneObject that contains a SkinnedMesh3DRenderer component,
 * but return just the SkinnedMesh3DRenderer instance.
 */
SkinnedMesh3DRendererRef Game::FindFirstSkinnedMeshRenderer(SceneObjectRef ref)
{
	if(!ref.IsValid())return SkinnedMesh3DRendererRef::Null();

	if(ref->GetSkinnedMesh3DRenderer().IsValid())return ref->GetSkinnedMesh3DRenderer();

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		SceneObjectRef childRef = ref->GetChildAt(i);
		SkinnedMesh3DRendererRef subRef = FindFirstSkinnedMeshRenderer(childRef);
		if(subRef.IsValid())return subRef;
	}

	return SkinnedMesh3DRendererRef::Null();
}

/*
 * Recursively search the scene hierarchy starting at [ref] for an
 * instance of SceneObject that contains a Mesh3D component, and return
 * the containing SceneObject instance.
 */
SceneObjectRef Game::FindFirstSceneObjectWithMesh(SceneObjectRef ref)
{
	if(!ref.IsValid())return SceneObjectRef::Null();

	if(ref->GetMesh3D().IsValid())return ref;

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		SceneObjectRef childRef = ref->GetChildAt(i);
		SceneObjectRef subRef = FindFirstSceneObjectWithMesh(childRef);
		if(subRef.IsValid())return subRef;
	}

	return SceneObjectRef::Null();
}

/*
 * Recursively visit objects in the scene hierarchy with root at [ref] and for each
 * invoke [func] with the current SceneObject instance as the only parameter.
 */
void Game::ProcessSceneObjects(SceneObjectRef ref, std::function<void(SceneObjectRef)> func)
{
	if(!ref.IsValid())return;

	// invoke [func]
	func(ref);

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		SceneObjectRef childRef = ref->GetChildAt(i);
		ProcessSceneObjects(childRef, func);
	}
}

/*
 * Set the SceneObject [root] and all children to be static.
 */
void Game::SetAllObjectsStatic(SceneObjectRef root)
{
	ProcessSceneObjects(root, [=](SceneObjectRef current)
	{
		current->SetStatic(true);
	});
}

/*
 * Set the SceneObject [root] and all children to have layer mask [mask].
 */
void Game::SetAllObjectsLayerMask(SceneObjectRef root, IntMask mask)
{
	ProcessSceneObjects(root, [=](SceneObjectRef current)
	{
		current->SetLayerMask(mask);
	});
}

/*
 * Set any mesh encountered in the scene hierarchy beginning with [root]
 * to use the standard shadow volume generation algorithm.
 */
void Game::SetAllMeshesStandardShadowVolume(SceneObjectRef root)
{
	ProcessSceneObjects(root, [=](SceneObjectRef current)
	{
		SkinnedMesh3DRendererRef skinnedRenderer = current->GetSkinnedMesh3DRenderer();
		if(skinnedRenderer.IsValid())
		{
			for(unsigned int i = 0; i < skinnedRenderer->GetSubRendererCount(); i++)
			{
				skinnedRenderer->GetSubRenderer(i)->SetUseBackSetShadowVolume(false);
			}
		}

		Mesh3DRendererRef renderer = current->GetMesh3DRenderer();
		if(renderer.IsValid())
		{
			for(unsigned int i = 0; i < renderer->GetSubRendererCount(); i++)
			{
				renderer->GetSubRenderer(i)->SetUseBackSetShadowVolume(false);
			}
		}
	});
}

/*
 * Set any mesh encountered in the scene hierarchy beginning with [root]
 * to set shadow casting to [castShadows].
 */
void Game::SetAllObjectsCastShadows(SceneObjectRef root, bool castShadows)
{
	ProcessSceneObjects(root, [=](SceneObjectRef current)
	{
		Mesh3DFilterRef filter = current->GetMesh3DFilter();
		if(filter.IsValid())
		{
			filter->SetCastShadows(castShadows);
		}
	});
}

/*
 * Initialize the game. This method calls functions to set up the game camera, load & set up scenery,
 * load the player model & animations, and set up the scene lights.
 */
void Game::Init()
{
	// instantiate an asset importer to load models
	AssetImporter importer;

	LayerManager& layerManager = Engine::Instance()->GetEngineObjectManager()->GetLayerManager();
	int lavaWallLayerIndex = layerManager.AddLayer(LavaWallLayer);
	int lavaIslandLayerIndex = layerManager.AddLayer(LavaIslandLayer);
	int lavaIslandObjectsLayerIndex = layerManager.AddLayer(LavaIslandObjectsLayer);
	int playerObjectLayerIndex = layerManager.AddLayer(PlayerObjectLayer);

	lavaWallLayerMask = layerManager.GetLayerMask(lavaWallLayerIndex);
	lavaIslandLayerMask = layerManager.GetLayerMask(lavaIslandLayerIndex);
	lavaIslandObjectsLayerMask = layerManager.GetLayerMask(lavaIslandObjectsLayerIndex);
	playerObjectLayerMask = layerManager.GetLayerMask(playerObjectLayerIndex);

	// set up the scene
	SetupScene(importer);
	SetupLights(importer);

	// setup & initialize player
	SetupPlayer(importer);
	InitializePlayerPosition();

	SetupCamera();
}

/*
 * Set up the scenery for the scene and use [importer] to load assets from disk. This function
 * uses sub-functions to do the grunt work of loading each required model, extracting relevant
 * components that can be reused (meshes & materials) and positions each instance of each model
 * in the scene.
 */
void Game::SetupScene(AssetImporter& importer)
{
	SetupSceneTerrain(importer);
	SetupSceneStructures(importer);
	SetupScenePlants(importer);
	SetupSceneExtra(importer);
}

/*
 * Set up the main camera for the scene.
 */
void Game::SetupCamera()
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// create camera
	cameraObject = objectManager->CreateSceneObject();
	CameraRef camera = objectManager->CreateCamera();
	cameraObject->SetCamera(camera);

	// specify which kinds of render buffers to use for this camera
	camera->AddClearBuffer(RenderBufferType::Color);
	camera->AddClearBuffer(RenderBufferType::Depth);

	// move camera object to its initial position
	Vector3 trans;
	Vector3 scale;
	Quaternion rot;
	Matrix4x4 mat;

	// decompose player's transform into position, rotation and scale
	playerObject->GetTransform().CopyMatrix(mat);
	mat.Decompose(trans,rot,scale);
	cameraObject->GetTransform().Translate(trans.x-20,trans.y+10,trans.z+15, true);

	// create skybox texture
	TextureRef skyboxTexture = objectManager->CreateCubeTexture("resources/textures/skybox-night/nightsky_north.png",
														 	 	"resources/textures/skybox-night/nightsky_south.png",
														 	 	"resources/textures/skybox-night/nightsky_up.png",
														 	 	"resources/textures/skybox-night/nightsky_down.png",
														 	 	"resources/textures/skybox-night/nightsky_east.png",
														 	 	"resources/textures/skybox-night/nightsky_west.png");
	// activate skybox
	camera->SetSkybox(skyboxTexture);
	camera->SetSkyboxEnabled(true);
}

/*
 * Set up the "land" elements in the scene.
 */
void Game::SetupSceneTerrain(AssetImporter& importer)
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
	SetAllObjectsStatic(modelSceneObject);
	SetAllObjectsLayerMask(modelSceneObject, lavaIslandLayerMask);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.04,.04, false);
	modelSceneObject->GetTransform().Translate(-30,-10,0,false);

	// load castle island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load island model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.07,.07,.07, false);
	modelSceneObject->GetTransform().Translate(80,-10,-10,false);

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
	SetAllObjectsStatic(modelSceneObject);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);

	// extract mesh & material from stone model
	SceneObjectRef stone1MeshObject = FindFirstSceneObjectWithMesh(modelSceneObject);
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
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 1, 0,0, -90, -16,-15,-55, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 0, 1,0, -70, -54,-30,-52, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-15,false);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 1, 0,0, -90, -66,-15,-27, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-90, true);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 0, 1,0, -20, -95,-17, 8, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .55,.58, .55, 1, 0,0, -90, -90,-15, 30, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-40, true);
	modelSceneObject->GetTransform().Rotate(1,0,0,-180, true);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .5,.5, .5, 0, 1,0, -80, -85,-28, 43, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .45,.4, .45, 1, 0,0, -90, -40,-25, 45, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject->GetTransform().Rotate(0,0,1,-40, true);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .6,.5, .5, 0, 1,0, -80, -20,-28, 45, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .5,.6, .6, 0, 1,0, -180, 10,-31.5, 5, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);
	modelSceneObject = AddMeshToScene(stone1Mesh, stone1Material, .25,.6, .25, 0, 1,0, -180, 10,-25, -25, true,true,true);
	SetAllObjectsLayerMask(modelSceneObject, lavaWallLayerMask);

}

/*
 * Set up all the man-made structures, buildings, etc. in the scene.
 */
void Game::SetupSceneStructures(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Castle components
	//
	//========================================================

	// load turrent tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tower model!\n");
	SetAllObjectsStatic(modelSceneObject);
	SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place turret tower in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.05,.05,.05, false);
	modelSceneObject->GetTransform().Translate(-30,-10,-10,false);

	// load castle tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_02.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tower model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle tower model
	SceneObjectRef tower2MeshObject = FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef tower2Mesh = tower2MeshObject->GetMesh3D();
	Mesh3DRendererRef towerRenderer = tower2MeshObject->GetMesh3DRenderer();
	MaterialRef towerMaterial = towerRenderer->GetMaterial(0);

	// place initial castle tower in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.03,.04, false);
	modelSceneObject->GetTransform().Translate(65,-10,-15,false);

	// re-use the castle tower mesh & material for multiple instances
	AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 89,-10,-15, true,true,true);
	AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 65,-10,6, true,true,true);
	AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 89,-10,6, true,true,true);

	// load & place castle entrance arch-way left side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Left_02.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load wall model!\n");
	SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.04,.04, false);
	modelSceneObject->GetTransform().Rotate(0,1,0,90,false);
	modelSceneObject->GetTransform().Translate(65,-10,-8.8,false);

	// load and place castle entrance arch-way right side
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Right_02.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load wall model!\n");
	SetAllObjectsStatic(modelSceneObject);
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.04,.04,.04, false);
	modelSceneObject->GetTransform().Rotate(0,1,0, 90,false);
	modelSceneObject->GetTransform().Translate(65,-10,0,false);

	// load castle wall model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Wall_Block_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load wall model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// extract mesh & material from castle wall model
	SceneObjectRef wallBlockMeshObject = FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef wallBlockMesh = wallBlockMeshObject->GetMesh3D();
	Mesh3DRendererRef wallBlockRenderer = wallBlockMeshObject->GetMesh3DRenderer();
	MaterialRef wallBlockMaterial = wallBlockRenderer->GetMaterial(0);

	// place initial castle wall in scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.06,.05,.04, false);
	modelSceneObject->GetTransform().Translate(70,-10,6.5,false);

	// re-use the castle wall mesh & material for multiple instances
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 78,-10,6.5, true,true,true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 86,-10,6.5, true,true,true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 70,-10,-15.5, true,true,true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 78,-10, -15.5, true,true,true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 86,-10, -15.5, true,true,true);
	modelSceneObject = AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.067,.05, 1,0,0, -90, 90,-10, -9.25, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);
	modelSceneObject = AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.067,.05, 1,0,0, -90, 90,-10, .25, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);


	//========================================================
	//
	// Mushroom house
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/mushroom/MushRoom_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load mushroom house model!\n");
	SetAllObjectsStatic(modelSceneObject);
	SetAllObjectsLayerMask(modelSceneObject, lavaIslandObjectsLayerMask);

	// place mushroom house in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.09,.09,.09, false);
	modelSceneObject->GetTransform().Translate(-37,-10,15,false);
}

/*
 * Add all the plants to the scene.
 */
void Game::SetupScenePlants(AssetImporter& importer)
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
	SetAllObjectsStatic(modelSceneObject);
	SetAllMeshesStandardShadowVolume(modelSceneObject);

	// extract tree mesh & material
	SceneObjectRef treeMeshObject = FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef treeMesh = treeMeshObject->GetMesh3D();
	Mesh3DRendererRef treeRenderer = treeMeshObject->GetMesh3DRenderer();
	MaterialRef treeMaterial = treeRenderer->GetMaterial(0);

	// place initial tree in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.0015,.0015,.0015, false);
	modelSceneObject->GetTransform().Rotate(.8,0,.2, -6, false);
	modelSceneObject->GetTransform().Translate(55,-10.5, 11,false);

	// reuse the tree mesh & material for multiple instances
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .10,.10,.10, 1,0,0, -85, 57, -10, 24, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .15,.15,.20, 1,0,0, -94, 61, -9, -15, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .20,.20,.30, 1,0,0, -93, 80, -9, -15, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .17,.17,.20, 1,0,0, -85, 85, -9.5, -13, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .22,.22,.38, 1,0,0, -90, 115, -9.5, 15, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .19,.19,.28, 1,0,0, -96, 105, -9.5, 8, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .18,.18,.20, 1,0,0, -87, 95, -10, 32, true,true,true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
}

/*
 * Add miscellaneous elements to the scene.
 */
void Game::SetupSceneExtra(AssetImporter& importer)
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// misc. reference variables
	SceneObjectRef modelSceneObject;
	Mesh3DRendererRef renderer;
	SceneObjectRef sceneObject;
	TextureAttributes texAttributes;
	TextureRef texture;
	Mesh3DRef cubeMesh;
	StandardAttributeSet meshAttributes;

	//========================================================
	//
	// Texture scene cube
	//
	//========================================================

	// create instance of SceneObject to hold the cube mesh and its renderer
	cubeSceneObject = objectManager->CreateSceneObject();
	SetAllObjectsLayerMask(cubeSceneObject, lavaIslandObjectsLayerMask);

	// load texture for the cube
	texAttributes.FilterMode = TextureFilter::TriLinear;
	texAttributes.MipMapLevel = 4;
	texture = objectManager->CreateTexture("resources/textures/cartoonTex03.png", texAttributes);

	// create the cube's material using the "basic" built-in shader
	ShaderSource basicShaderSource;
	importer.LoadBuiltInShaderSource("basic", basicShaderSource);
	MaterialRef material = objectManager->CreateMaterial(std::string("BasicMaterial"), basicShaderSource);
	material->SetTexture(texture, "TEXTURE0");

	// set the cube mesh attributes
	meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);

	// create the cube mesh
	cubeMesh = EngineUtility::CreateCubeMesh(meshAttributes);
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(cubeMesh);
	filter->SetCastShadows(true);
	filter->SetReceiveShadows(true);

	// create the cube mesh's renderer
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetMesh3DRenderer(renderer);

	// scale the cube and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(1.5, 1.5,1.5, false);
	cubeSceneObject->GetTransform().Translate(-20, -7, 8, false);


	//========================================================
	//
	// Fences
	//
	//========================================================

	// load fence model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrier01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load barrier model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// extract fence mesh & material
	SceneObjectRef fenceMeshObject = FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef fenceMesh = fenceMeshObject->GetMesh3D();
	Mesh3DRendererRef fenceRenderer = fenceMeshObject->GetMesh3DRenderer();
	MaterialRef fenceMaterial = fenceRenderer->GetMaterial(0);

	// place initial fence in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.6,.6,.6, false);
	modelSceneObject->GetTransform().Translate(60,-10,22,false);

	/** re-use the fence mesh & material for multiple instances  **/

	// fence on right side of castle
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 47,-10,14.5, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-70,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 52,-10,20, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-25,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 69.5,-10,21, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,15,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 79.2,-10,19.3, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,5,true);

	// fence on left side of castle
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 51,-10,-16, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-120,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 55,-10,-25, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-110,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 59.9,-10,-33.3, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-135,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 68.2,-10,-38, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-160,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 78,-10,-40, true,true,true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-175,true);


	//========================================================
	//
	// Barrels
	//
	//========================================================

	// load barrel model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/wood/Barrel01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load barrel model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// extract barrel mesh and material
	SceneObjectRef barrelMeshObject = FindFirstSceneObjectWithMesh(modelSceneObject);
	Mesh3DRef barrelMesh = barrelMeshObject->GetMesh3D();
	Mesh3DRendererRef barrelRenderer = barrelMeshObject->GetMesh3DRenderer();
	MaterialRef barrelMaterial = barrelRenderer->GetMaterial(0);

	// place initial barrel in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.8,.8,.8, false);
	modelSceneObject->GetTransform().Translate(78,-10,10.5, false);

	// re-use the barrel mesh & material for multiple instances
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .8,.8,.8, 1,0,0, -90, 74,-10,10.5, true,true,true);
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-8.3,1.5, true,true,true);
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-8.3,-1.8, true,true,true);
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-5.3,-.15, true,true,true);
}

/*
 * Set up the lights in the scene.
 */
void Game::SetupLights(AssetImporter& importer)
{
	SceneObjectRef sceneObject;

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
	selflitMaterial->SetColor(Color4(1,1,1,1), "SELFCOLOR");

	// create spinning point light
	spinningPointLightObject = objectManager->CreateSceneObject();
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
	light->MergeCullingMask(playerObjectLayerMask);
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Point);
	spinningPointLightObject->SetLight(light);
	spinningPointLightObject->GetTransform().Scale(.4,.4,.4, true);
	spinningPointLightObject->GetTransform().Translate(-26, 2, 10, false);

	//========================================================
	//
	// Ambient light
	//
	//========================================================

	// create ambient light
	ambientLightObject = objectManager->CreateSceneObject();
	light = objectManager->CreateLight();
	light->SetIntensity(.30);
	light->SetType(LightType::Ambient);
	light->MergeCullingMask(playerObjectLayerMask);
	light->MergeCullingMask(lavaIslandLayerMask);
	light->MergeCullingMask(lavaIslandObjectsLayerMask);
	light->MergeCullingMask(lavaWallLayerMask);
	ambientLightObject->SetLight(light);


	//========================================================
	//
	// Directional light
	//
	//========================================================

	directionalLightObject = objectManager->CreateSceneObject();
	directionalLightObject->SetStatic(true);
	light = objectManager->CreateLight();
	light->SetDirection(-.8,-1.7,-2);
	light->SetIntensity(.8);
	light->MergeCullingMask(playerObjectLayerMask);
	light->MergeCullingMask(lavaIslandLayerMask);
	light->MergeCullingMask(lavaIslandObjectsLayerMask);
	light->MergeCullingMask(lavaWallLayerMask);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Directional);
	directionalLightObject->SetLight(light);


	//========================================================
	//
	// Castle lights
	//
	//========================================================

	// create mesh & material for castle lantern
	Mesh3DRef lanternLightMesh = EngineUtility::CreateCubeMesh(meshAttributes);
	Color4 lanternLightColor(1,.66,.231,1);
	Color4 lanternLightMeshColor(1,.95,.5,1);
	MaterialRef lanterLightMeshMaterial = objectManager->CreateMaterial("LanternLightMeshMaterial", selfLitShaderSource);
	lanterLightMeshMaterial->SetColor(lanternLightMeshColor, "SELFCOLOR");
	lanterLightMeshMaterial->SetSelfLit(true);

	// create castle right lantern
	SceneObjectRef lanternObject = objectManager->CreateSceneObject();
	lanternObject->SetStatic(true);
	LightRef lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.8);
	lanternLight->SetRange(25);
	lanternLight->SetColor(lanternLightColor);
	lanternLight->MergeCullingMask(playerObjectLayerMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2,.2,.2, true);
	lanternObject->GetTransform().Translate(62.2, -5, 0, false);
	filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	Mesh3DRendererRef lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
	otherPointLightObjects.push_back(lanternObject);

	// create castle left lantern
	lanternObject = objectManager->CreateSceneObject();
	lanternObject->SetStatic(true);
	lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.8);
	lanternLight->SetRange(25);
	lanternLight->SetColor(lanternLightColor);
	lanternLight->MergeCullingMask(playerObjectLayerMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2,.2,.2, true);
	lanternObject->GetTransform().Translate(62.4, -5, -8, false);
	filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
	otherPointLightObjects.push_back(lanternObject);

	// create castle side lantern
	lanternObject = objectManager->CreateSceneObject();
	lanternObject->SetStatic(true);
	lanternLight = objectManager->CreateLight();
	lanternLight->SetIntensity(1.5);
	lanternLight->SetRange(20);
	lanternLight->SetColor(lanternLightColor);
	lanternLight->MergeCullingMask(playerObjectLayerMask);
	lanternLight->SetShadowsEnabled(true);
	lanternLight->SetType(LightType::Point);
	lanternObject->SetLight(lanternLight);
	lanternObject->GetTransform().Scale(.2,.2,.2, true);
	lanternObject->GetTransform().Translate(77.4, -5, -17.8, false);
	filter = objectManager->CreateMesh3DFilter();
	lanternObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	lanterLightRenderer = objectManager->CreateMesh3DRenderer();
	lanterLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lanternObject->SetMesh3DRenderer(lanterLightRenderer);
	otherPointLightObjects.push_back(lanternObject);

	//========================================================
	//
	// Lava lights
	//
	//========================================================

	// create lava pool wall light
	SceneObjectRef lavaLightObject = objectManager->CreateSceneObject();
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
	filter = objectManager->CreateMesh3DFilter();
	lavaLightObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(lanternLightMesh);
	Mesh3DRendererRef lavaLightRenderer = objectManager->CreateMesh3DRenderer();
	lavaLightRenderer->AddMaterial(lanterLightMeshMaterial);
	lavaLightObject->SetMesh3DRenderer(lavaLightRenderer);
	lavaLightObjects.push_back(lavaLightObject);

	// create lava island light
	SceneObjectRef lavaDirectionalLightObject = objectManager->CreateSceneObject();
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

/*
 * Given an instance of Mesh3D [mesh] and and instance of Material [material], create an instance of SceneObject that
 * contains [mesh] and renders it using [material]. Scale the scene object's transform by [sx], [sy], [sz] in world
 * space, then rotate by [ra] degrees around the world space axis [rx], [ry], [rz], and then translate in world space by
 * [tx], [ty], tz]. If [isStatic] == true, then set the root SceneObject instance and all children to be static.
 *
 * This method is used to handle all the details of placing an arbitrary mesh somewhere in the scene at a specified orientation.
 */
SceneObjectRef Game::AddMeshToScene(Mesh3DRef mesh, MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra,
									float tx, float ty, float tz, bool isStatic, bool castShadows, bool receiveShadows)
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	SceneObjectRef meshSceneObject = objectManager->CreateSceneObject();
	meshSceneObject->SetActive(true);
	Mesh3DFilterRef meshFilter = objectManager->CreateMesh3DFilter();
	meshFilter->SetMesh3D(mesh);
	meshFilter->SetCastShadows(castShadows);
	meshFilter->SetReceiveShadows(receiveShadows);
	meshSceneObject->SetMesh3DFilter(meshFilter);
	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	meshSceneObject->SetMesh3DRenderer(renderer);

	meshSceneObject->GetTransform().Scale(sx,sy,sz, false);
	if(ra != 0)meshSceneObject->GetTransform().Rotate(rx,ry,rz,ra,false);
	meshSceneObject->GetTransform().Translate(tx,ty,tz,false);

	if(isStatic)SetAllObjectsStatic(meshSceneObject);

	return meshSceneObject;
}

/*
 * Set up the player model and animations, use [importer] to load model files from disk.
 */
void Game::SetupPlayer(AssetImporter& importer)
{
	SkinnedMesh3DRendererRef playerMeshRenderer;
	Mesh3DRef firstMesh;
	playerType = PlayerType::Warrior;
	playerState = PlayerState::Waiting;
	playerIsGrounded = true;


	//========================================================
	//
	// Load player model
	//
	//========================================================

	switch(playerType)
	{
		case PlayerType::Koopa:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);
			playerObject = importer.LoadModelDirect("resources/models/koopa/koopamod.fbx");
			ASSERT_RTRN(playerObject.IsValid(), "Could not load Koopa model!\n");
			playerObject->GetTransform().SetIdentity();
			playerObject->GetTransform().Translate(50,-10,-2,false);
			playerObject->GetTransform().Scale(.05, .05, .05, true);
		break;
		case PlayerType::Warrior:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, true);
			playerObject = importer.LoadModelDirect("resources/models/toonwarrior/character/warrior.fbx");
			ASSERT_RTRN(playerObject.IsValid(), "Could not load Warrior model!\n");
			playerObject->GetTransform().Translate(50,-10,-10,false);
			playerObject->GetTransform().Scale(4, 4, 4, true);
		break;
	}


	//========================================================
	//
	// Load player animations
	//
	//========================================================

	switch(playerType)
	{
		case PlayerType::Koopa:
			playerAnimations[PlayerState::Waiting] = importer.LoadAnimation("resources/models/koopa/model/koopa@wait.fbx", true);
			playerAnimations[PlayerState::Walking]  = importer.LoadAnimation("resources/models/koopa/model/koopa@walk.fbx", true);
			playerAnimations[PlayerState::Roaring]  = importer.LoadAnimation("resources/models/koopa/model/koopa@roar3.fbx", false);
			playerAnimations[PlayerState::Jump]  = importer.LoadAnimation("resources/models/koopa/model/koopa@jump.fbx", false);
			playerAnimations[PlayerState::JumpStart]  = importer.LoadAnimation("resources/models/koopa/model/koopa@jumpstart.fbx", false);
			playerAnimations[PlayerState::JumpEnd]  = importer.LoadAnimation("resources/models/koopa/model/koopa@jumpend.fbx", false);
			playerAnimations[PlayerState::JumpFall]  = importer.LoadAnimation("resources/models/koopa/model/koopa@jumpfall.fbx", false);
		break;
		case PlayerType::Warrior:
			playerAnimations[PlayerState::Waiting] = importer.LoadAnimation("resources/models/toonwarrior/animations/idle.fbx", true);
			playerAnimations[PlayerState::Walking] = importer.LoadAnimation("resources/models/toonwarrior/animations/walk.fbx", true);
			playerAnimations[PlayerState::Attack1] = importer.LoadAnimation("resources/models/toonwarrior/animations/slash1.fbx", true);
			playerAnimations[PlayerState::Attack2] = importer.LoadAnimation("resources/models/toonwarrior/animations/slash2.fbx", true);
			playerAnimations[PlayerState::Attack3] = importer.LoadAnimation("resources/models/toonwarrior/animations/slash4.fbx", true);
			playerAnimations[PlayerState::Defend1] = importer.LoadAnimation("resources/models/toonwarrior/animations/shield.fbx", true);
		break;
	}

	playerObject->SetActive(true);
	SetAllObjectsLayerMask(playerObject, playerObjectLayerMask);
	playerRenderer = FindFirstSkinnedMeshRenderer(playerObject);
	AnimationManager * animManager = Engine::Instance()->GetAnimationManager();
	bool compatible = true;


	//========================================================
	//
	// Set up player animations
	//
	//========================================================

	switch(playerType)
	{
		case PlayerType::Koopa:
			compatible = animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Walking]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Waiting]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Jump] );
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Roaring]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::JumpStart]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::JumpEnd]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::JumpFall]);

			ASSERT_RTRN(compatible, "Koopa animations are not compatible!");

			// create an animation player and some animations to it for the player object.
			playerAnimations[PlayerState::JumpFall]->ClipEnds(playerAnimations[PlayerState::JumpFall]->GetDuration() - .05, playerAnimations[PlayerState::JumpFall]->GetDuration());
			animationPlayer = animManager->RetrieveOrCreateAnimationPlayer(playerRenderer);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Waiting]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Walking]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Jump]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::JumpStart]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::JumpEnd]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::JumpFall]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Roaring]);
			animationPlayer->SetSpeed(playerAnimations[PlayerState::Walking], 2);
			animationPlayer->SetSpeed(playerAnimations[PlayerState::JumpStart], 4);
			animationPlayer->SetPlaybackMode(playerAnimations[PlayerState::JumpFall], PlaybackMode::Clamp);
			animationPlayer->Play(playerAnimations[PlayerState::Waiting]);

		break;
		case PlayerType::Warrior:
			compatible = animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Waiting]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Walking]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Attack1]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Attack2]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Attack3]);
			compatible &= animManager->IsCompatible(playerRenderer, playerAnimations[PlayerState::Defend1]);

			ASSERT_RTRN(compatible, "Warrior animations are not compatible!");

			// set all meshes to use standard shadow volume
			ProcessSceneObjects(playerObject, [=](SceneObjectRef current)
			{
				SkinnedMesh3DRendererRef renderer = current->GetSkinnedMesh3DRenderer();
				if(renderer.IsValid())
				{
					for(unsigned int i = 0; i < renderer->GetSubRendererCount(); i++)
					{
						renderer->GetSubRenderer(i)->SetUseBackSetShadowVolume(false);
					}
				}
			});

			// create an animation player and some animations to it for the player object.
			animationPlayer = animManager->RetrieveOrCreateAnimationPlayer(playerRenderer);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Waiting]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Walking]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Attack1]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Attack2]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Attack3]);
			animationPlayer->AddAnimation(playerAnimations[PlayerState::Defend1]);
			animationPlayer->SetSpeed(playerAnimations[PlayerState::Attack3], .65);
			animationPlayer->Play(playerAnimations[PlayerState::Waiting]);
		break;
	}
}



/*
 * Initialize the player's initial position
 */
void Game::InitializePlayerPosition()
{
	Vector3 playerForward = basePlayerForward;
	playerObject->GetTransform().TransformVector(playerForward);
	playerForward.y = 0;
	playerForward.Normalize();

	playerLookDirection = playerForward;

	Quaternion currentRotation;
	Vector3 currentTranslation;
	Vector3 currentScale;

	playerObject->GetTransform().GetLocalComponents(currentTranslation, currentRotation, currentScale);
	playerBaseY = currentTranslation.y;
}

/*
 * Update() is called once per frame
 */
void Game::Update()
{
	Point3 lightRotatePoint(-20,5,10);

	// rotate the point light around [lightRotatePoint]
	spinningPointLightObject->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z,0,1,0,60 * Time::GetDeltaTime(), false);
	//pointLightObject->SetActive(false);

	// rotate the cube around its center and the y-axis
	cubeSceneObject->GetTransform().Rotate(0,1,0,20 * Time::GetDeltaTime(), true);

	UpdatePlayerHorizontalSpeedAndDirection();
	UpdatePlayerVerticalSpeed();
	ApplyPlayerMovement();
	UpdatePlayerAnimation();
	UpdatePlayerLookDirection();
	UpdatePlayerFollowCamera();
	ManagePlayerState();
	HandleGeneralInput();

	if(lavaField->GetSceneObject()->IsActive())lavaField->Update();

	DisplayInfo();
}

/*
 * Display info for the demo:
 *
 *   - The graphics engine frames-per-second
 *   - Currently active scene lighting
 */
void Game::DisplayInfo()
{
	float elapsedPrintTime = Time::GetRealTimeSinceStartup() - lastInfoPrintTime;
	if(elapsedPrintTime > 1 || displayInfoChanged)
	{
		float elapsedFPSTime = Time::GetRealTimeSinceStartup() - lastFPSRetrieveTime;
		if(elapsedFPSTime > 1)
		{
			lastFPS = Engine::Instance()->GetGraphicsEngine()->GetCurrentFPS();
			lastFPSRetrieveTime = Time::GetRealTimeSinceStartup();
		}

		printf("FPS: %f ", lastFPS);

		switch(selectedLighting)
		{
			case SceneLighting::Ambient:
				printf(" |  Selected lighting: Ambient");
			break;
			case SceneLighting::Directional:
				printf(" |  Selected lighting: Directional");
			break;
			case SceneLighting::Point:
				printf(" |  Selected lighting: Point");
			break;
			case SceneLighting::Lava:
				printf(" |  Selected lighting: Lava lights");
			break;
			default:
				printf(" |  Selected lighting: None");
			break;
		}

		printf("                             \r");
		fflush(stdout);

		lastInfoPrintTime = Time::GetRealTimeSinceStartup();
	}

	displayInfoChanged = false;
}

/*
 * Signal that the info that is displayed to the user has been updated.
 */
void Game::SignalDisplayInfoChanged()
{
	displayInfoChanged = true;
}

/*
 * Update the direction in which the player is moving and the player's
 * move speed based on user input.
 */
void Game::UpdatePlayerHorizontalSpeedAndDirection()
{
	float curSmooth = playerSpeedSmoothing * Time::GetDeltaTime();

	if(playerState == PlayerState::Roaring ||
	   playerState == PlayerState::Defend1 ||
	   playerState == PlayerState::Attack1 ||
	   playerState == PlayerState::Attack2)
	{
		playerHorizontalSpeed = GTEMath::Lerp(playerHorizontalSpeed, 0, curSmooth);
		return;
	}

	Point3 cameraPos;
	Point3 playerPos;

	// convert camera position from local to world space
	cameraObject->GetTransform().TransformPoint(cameraPos);

	// convert player position from local to world space
	playerObject->GetTransform().TransformPoint(playerPos);

	// convert player forward vector from local to world space
	Vector3 playerForward = basePlayerForward;
	playerObject->GetTransform().TransformVector(playerForward);
	playerForward.y = 0;
	playerForward.Normalize();

	// convert camera forward vector from local to world space
	Vector3 cameraForward = baseCameraForward;
	cameraObject->GetTransform().TransformVector(cameraForward);
	cameraForward.y = 0;
	cameraForward.Normalize();

	// calculate the vector that is 90 degrees to the player's right
	Vector3 cameraRight;
	Vector3::Cross(cameraForward, Vector3::Up, cameraRight);

	float h = 0;
	float v = 0;
	InputManager * inputManager = Engine::Instance()->GetInputManager();

	// get directional input
	if(inputManager->GetDigitalInputState(DigitalInput::Left))h -= 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Right))h += 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Up))v += 1;
	if(inputManager->GetDigitalInputState(DigitalInput::Down))v -= 1;

	playerIsMoving = GTEMath::Abs(h) > .1 || GTEMath::Abs(v) > .1;

	// scale right vector according to horizontal input
	Vector3 cameraRightScaled = cameraRight;
	cameraRightScaled.Scale(h);

	// scale forward vector according to vertical input
	Vector3 cameraForwardScaled = cameraForward;
	cameraForwardScaled.Scale(v);

	Vector3 targetDirection;

	// add scaled vectors to get final target facing vector
	Vector3::Add(cameraRightScaled, cameraForwardScaled, targetDirection);
	targetDirection.Normalize();

	if(targetDirection.x != 0 || targetDirection.y != 0 || targetDirection.z != 0)
	{
		// rotate from the current facing vector to the target facing vector, instead of jumping directly to it to
		// create smooth rotation
		bool success = Vector3::RotateTowards(playerLookDirection, targetDirection,  playerRotateSpeed * Time::GetDeltaTime(), playerMoveDirection);

		// the RotateTowards() operation can fail if the 'from' and 'to' vectors are opposite (180 degrees from each other).
		// in such a case we create a new target direction that is 90 degrees from the current facing vector to
		// either the left or right (as appropriate).
		if(!success)
		{
			Vector3::Cross(Vector3::Up, playerLookDirection, targetDirection);
			Vector3::RotateTowards(playerLookDirection, targetDirection,  playerRotateSpeed * Time::GetDeltaTime(), playerMoveDirection);
		}

		playerLookDirection = playerMoveDirection;
		playerLookDirection.y = 0;
		playerLookDirection.Normalize();
	}

	// if the player is on the ground, apply movement
	if(playerIsGrounded)
	{
		float targetSpeed = 0;
		if(playerIsMoving)
		{
			targetSpeed = playerWalkSpeed;
		}
		playerHorizontalSpeed = GTEMath::Lerp(playerHorizontalSpeed, targetSpeed, curSmooth);
	}
}

/*
 * If the player is jumping or falling or not grounded in some way, manage their
 * vertical speed
 */
void Game::UpdatePlayerVerticalSpeed()
{
	Quaternion currentRotation;
	Vector3 currentTranslation;
	Vector3 currentScale;

	playerJumpApexReached = false;
	playerLanded = false;

	// get the current position, rotation, and scale
	playerObject->GetTransform().GetLocalComponents(currentTranslation, currentRotation, currentScale);


	// is the player currently moving upwards (y velocity > 0) ?
	bool movingUp = playerVerticalSpeed > 0;

	// if the player is currently on the ground, but is in the jump state
	// we set the player's Y velocity to a positive number if the player
	// has been in the jump state for a sufficient amount of time
	if(playerIsGrounded && playerState == PlayerState::Jump)
	{
		float jumpTime = Time::GetRealTimeSinceStartup() - stateActivationTime[(int) PlayerState::Jump] ;
		if(jumpTime > .2)
		{
			playerVerticalSpeed = 50;
			playerIsGrounded = false;
		}
	}

	// apply gravity to the player's Y velocity
	if(!playerIsGrounded)playerVerticalSpeed -= 95 * Time::GetDeltaTime();

	// if the player was moving upwards but now is not after the application
	// of gravity, then the jump's apex has been reached.
	if(playerVerticalSpeed <= 0 && movingUp)
	{
		playerJumpApexReached = true;
	}

	// check if the player has landed from the jump or fall
	if(currentTranslation.y < playerBaseY + 1 && playerVerticalSpeed < 0 && !playerIsGrounded)
	{
		playerObject->GetTransform().GetLocalComponents(currentTranslation, currentRotation, currentScale);
		currentTranslation.y = playerBaseY;
		playerObject->GetTransform().SetLocalComponents(currentTranslation, currentRotation, currentScale);
		playerVerticalSpeed = 0;
		playerIsGrounded = true;
		playerLanded = true;
	}
}
/*
 * Update the player's position base on its current horizontal speed, vertical speed, and facing direction.
 */
void Game::ApplyPlayerMovement()
{
	// apply player's Y velocity
	if(!playerIsGrounded)
	{
		Vector3 move(0, playerVerticalSpeed * Time::GetDeltaTime(), 0);
		playerObject->GetTransform().Translate(move, false);
	}

	// apply horizontal (x-z) movement
	if(playerHorizontalSpeed > .1)
	{
		Vector3 move = playerLookDirection;
		move.Scale(playerHorizontalSpeed * Time::GetDeltaTime());
		playerObject->GetTransform().Translate(move, false);
	}
}

/*
 * Update the player object to have its forward vector aligned with [lookDirection]
 */
void Game::UpdatePlayerLookDirection()
{
	if(playerState == PlayerState::Roaring ||
	   playerState == PlayerState::Defend1 ||
	   playerState == PlayerState::Attack1 ||
	   playerState == PlayerState::Attack2)return;

	// axis around which to rotate player object
	Vector3 rotationAxis(0,1,0);

	// get a quaternion that represents the rotation from the player object's original forward vector
	// to [lookDirection] in world space.
	Quaternion modRotation = Quaternion::getRotation(basePlayerForward, playerLookDirection, rotationAxis);
	modRotation.normalize();

	Quaternion currentRotation;
	Vector3 currentTranslation;
	Vector3 currentScale;

	// apply the quaternion calculated above
	playerObject->GetTransform().GetLocalComponents(currentTranslation, currentRotation, currentScale);
	playerObject->GetTransform().SetLocalComponents(currentTranslation, modRotation, currentScale);
}

/*
 * Update the player's active animation based on its current action.
 */
void Game::UpdatePlayerAnimation()
{
	switch(playerType)
	{
		case PlayerType::Koopa:
			if(playerState == PlayerState::Walking)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Walking], .2);
			else if(playerState == PlayerState::Waiting)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Waiting], .3);
			else if(playerState == PlayerState::Roaring)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Roaring], .2);
			else if(playerState == PlayerState::JumpStart)
				animationPlayer->CrossFade(playerAnimations[PlayerState::JumpStart], .1);
			else if(playerState == PlayerState::Jump)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Jump], .2);
			else if(playerState == PlayerState::JumpFall)
				animationPlayer->CrossFade(playerAnimations[PlayerState::JumpFall], .4);
			else if(playerState == PlayerState::JumpEnd)
				animationPlayer->CrossFade(playerAnimations[PlayerState::JumpEnd], .05);
		break;
		case PlayerType::Warrior:
			if(playerState == PlayerState::Walking)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Walking], .2);
			else if(playerState == PlayerState::Waiting)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Waiting], .3);
			else if(playerState == PlayerState::Attack1)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Attack1], .2);
			else if(playerState == PlayerState::Attack2)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Attack2], .2);
			else if(playerState == PlayerState::Attack3)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Attack3], .2);
			else if(playerState == PlayerState::Defend1)
				animationPlayer->CrossFade(playerAnimations[PlayerState::Defend1], .2);
		break;
	}
}

/*
 * Manage input, state activation, and transitions between states for the player.
 */
void Game::ManagePlayerState()
{
	float currentStateTime = Time::GetRealTimeSinceStartup() - stateActivationTime[(unsigned int)playerState];
	switch(playerType)
	{
		case PlayerType::Koopa:
			if(Engine::Instance()->GetInputManager()->IsKeyDown(Key::C))
			{
				if(playerState == PlayerState::Waiting || playerState == PlayerState::Walking)
					ActivatePlayerState(PlayerState::Roaring);
			}

			if(Engine::Instance()->GetInputManager()->IsKeyDown(Key::X))
			{
				if(playerState == PlayerState::Waiting || playerState == PlayerState::Walking)
					ActivatePlayerState(PlayerState::JumpStart);
			}

			if(playerState == PlayerState::Roaring && currentStateTime > 6)ActivatePlayerState(PlayerState::Waiting);
			if(playerState == PlayerState::JumpStart && currentStateTime >.2)ActivatePlayerState(PlayerState::Jump);
			if(playerJumpApexReached)ActivatePlayerState(PlayerState::JumpFall);
			if(playerLanded)ActivatePlayerState(PlayerState::JumpEnd);

			if(playerState == PlayerState::JumpEnd)
			{
				if(currentStateTime > .1 && playerHorizontalSpeed > .3)ActivatePlayerState(PlayerState::Walking);
				else if(currentStateTime > .3)ActivatePlayerState(PlayerState::Waiting);
			}

			if(playerState == PlayerState::Walking || playerState == PlayerState::Waiting)
			{
				if(playerHorizontalSpeed > .1)ActivatePlayerState(PlayerState::Walking);
				else ActivatePlayerState(PlayerState::Waiting);
			}
		break;
		case PlayerType::Warrior:

			if(Engine::Instance()->GetInputManager()->IsKeyDown(Key::X))
			{
				if(playerState == PlayerState::Waiting || playerState == PlayerState::Walking)
					ActivatePlayerState(PlayerState::Attack1);
			}

			if(Engine::Instance()->GetInputManager()->IsKeyDown(Key::C))
			{
				if(playerState == PlayerState::Waiting || playerState == PlayerState::Walking)
					ActivatePlayerState(PlayerState::Attack2);
			}

			if(Engine::Instance()->GetInputManager()->IsKeyDown(Key::V))
			{
				if(playerState == PlayerState::Waiting || playerState == PlayerState::Walking)
					ActivatePlayerState(PlayerState::Attack3);
			}

			if(Engine::Instance()->GetInputManager()->IsKeyDown(Key::B))
			{
				if(playerState == PlayerState::Waiting || playerState == PlayerState::Walking || playerState == PlayerState::Defend1 )
					ActivatePlayerState(PlayerState::Defend1);
			}

			if(playerState == PlayerState::Walking || playerState == PlayerState::Waiting)
			{
				if(playerHorizontalSpeed > .1)ActivatePlayerState(PlayerState::Walking);
				else ActivatePlayerState(PlayerState::Waiting);
			}

			if(playerState == PlayerState::Attack1 && currentStateTime > .5)ActivatePlayerState(PlayerState::Waiting);
			else if(playerState == PlayerState::Attack2 && currentStateTime > .45)ActivatePlayerState(PlayerState::Waiting);
			else if(playerState == PlayerState::Attack3 && currentStateTime > 1)ActivatePlayerState(PlayerState::Waiting);
			else if(playerState == PlayerState::Defend1 && currentStateTime > .45)ActivatePlayerState(PlayerState::Waiting);

		break;
	}
}

/*
 * Handle non-player input
 */
void Game::HandleGeneralInput()
{
	// toggle ssao
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::O))
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
		const GraphicsAttributes& graphicsAttributes = graphics->GetAttributes();
		graphics->SetSSAOEnabled(!graphicsAttributes.SSAOEnabled);
	}

	// toggle ssao render mode
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::I))
	{
		Graphics * graphics = Engine::Instance()->GetGraphicsEngine();
		const GraphicsAttributes& graphicsAttributes = graphics->GetAttributes();
		if(graphicsAttributes.SSAOMode == SSAORenderMode::Outline)graphics->SetSSAOMode(SSAORenderMode::Standard);
		else graphics->SetSSAOMode(SSAORenderMode::Outline);
	}

	// select ambient lights
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::A))
	{
		SignalDisplayInfoChanged();
		selectedLighting = SceneLighting::Ambient;

	}

	// select directional light
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::D))
	{
		SignalDisplayInfoChanged();
		selectedLighting = SceneLighting::Directional;

	}

	// select point lights
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::P))
	{
		SignalDisplayInfoChanged();
		selectedLighting = SceneLighting::Point;

	}

	// select lava lights
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::L))
	{
		SignalDisplayInfoChanged();
		selectedLighting = SceneLighting::Lava;

	}

	// toggle lava
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::K))
	{
		SceneObjectRef lavaFieldObject = lavaField->GetSceneObject();
		lavaFieldObject->SetActive(!lavaFieldObject->IsActive());
	}

	bool toggleCastShadows = Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::R);
	bool boostLightIntensity = Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::W);
	bool reduceLightIntensity =  Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::E);
	bool toggleLight = Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::Q);

	float intensityBoost = 0;
	if(boostLightIntensity)intensityBoost = .05;
	else if(reduceLightIntensity)intensityBoost = -.05;

	// update selected lights
	switch(selectedLighting)
	{
		case SceneLighting::Ambient:
			UpdateLight(ambientLightObject, toggleLight, intensityBoost, toggleCastShadows);
		break;
		case SceneLighting::Lava:
			for(unsigned int i =0; i < lavaLightObjects.size(); i++)
			{
				UpdateLight(lavaLightObjects[i], toggleLight, intensityBoost, toggleCastShadows);
			}
		break;
		case SceneLighting::Directional:
			UpdateLight(directionalLightObject, toggleLight, intensityBoost, toggleCastShadows);
		break;
		case SceneLighting::Point:
			UpdateLight(spinningPointLightObject, toggleLight, intensityBoost, toggleCastShadows);

			for(unsigned int i =0; i < otherPointLightObjects.size(); i++)
			{
				UpdateLight(otherPointLightObjects[i], toggleLight, intensityBoost, toggleCastShadows);
			}
		break;
		default:

		break;
	}

	// toggle skybox
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::S))
	{
		if(cameraObject.IsValid())
		{
			cameraObject->GetCamera()->SetSkyboxEnabled(!cameraObject->GetCamera()->IsSkyboxEnabled());
		}
	}
}

/*
 * Update the light that is attached to [sceneObject].
 *
 * [toggleLight] - Turn the light from on to off (or vice-versa).
 * [intensityChange] - Amount by which light intensity should be adjusted.
 * [toggleCastShadows] - Toggle whether or not shadows are enabled for the light.
 */
void Game::UpdateLight(SceneObjectRef sceneObject, bool toggleLight, float intensityChange, bool toggleCastShadows)
{
	if(sceneObject.IsValid())
	{
		if(toggleLight)sceneObject->SetActive(!sceneObject->IsActive());

		if(intensityChange != 0)
		{
			float intensity = sceneObject->GetLight()->GetIntensity();
			if(intensity + intensityChange < 0 && intensityChange < 0)intensityChange = -intensity;
			sceneObject->GetLight()->SetIntensity(intensity + intensityChange);
		}

		if(toggleCastShadows)
		{
			bool castShadows = sceneObject->GetLight()->GetShadowsEnabled();
			sceneObject->GetLight()->SetShadowsEnabled(!castShadows);
		}
	}
}

/*
 * Update the camera's position and direction that it is facing.
 */
void Game::UpdatePlayerFollowCamera()
{
	Point3 cameraPos;
	Point3 playerPos;

	// points that will determine the camera's final orientation. the camera
	// will first rotate to look at [playerPosCameraLookTargetA], and from there
	// will rotate to look at [playerPosCameraLookTargetB]
	Point3 playerPosCameraLookTargetA;
	Point3 playerPosCameraLookTargetB;
	// position to which the camera will move
	Point3 playerPosCameraMoveTarget;

	// get the gamera's position in world space into [cameraPos]
	cameraObject->GetTransform().TransformPoint(cameraPos);

	// get the player object's position in world space into [playerPos]
	playerObject->GetTransform().TransformPoint(playerPos);

	playerPosCameraLookTargetA = playerPos;
	playerPosCameraMoveTarget = playerPos;

	// set the values of the two points that will be used to form the camera's
	// orientation rotations
	playerPosCameraLookTargetA.y = cameraPos.y;
	playerPosCameraLookTargetB = playerPosCameraLookTargetA;
	playerPosCameraLookTargetB.y = playerPos.y + 10;

	// the target position to which the camera should move
	playerPosCameraMoveTarget.y = playerPos.y + 13;

	// vector that represent the camera's direction when looking at [playerPosCameraLookTargetA]
	Vector3 cameraToPlayerLookA;
	// vector that represent the camera's direction when looking at [playerPosCameraLookTargetB]
	Vector3 cameraToPlayerLookB;
	// get a vector from the camera's current position to [cameraToPlayerLookA] and store in [cameraToPlayerLookA]
	Point3::Subtract(playerPosCameraLookTargetA, cameraPos, cameraToPlayerLookA);
	// get a vector from the camera's current position to [cameraToPlayerLookB] and store in [cameraToPlayerLookB]
	Point3::Subtract(playerPosCameraLookTargetB, cameraPos, cameraToPlayerLookB);

	// get a vector from the camera's current position to its target position,
	// and store in [cameraToPlayerMove]
	Vector3 cameraToPlayerMove;
	Point3::Subtract(playerPosCameraMoveTarget, cameraPos, cameraToPlayerMove);

	// project [cameraToPlayerMove] into the x-z plane
	cameraToPlayerMove.y=0;

	// target distance camera should be from the player object. this means the real target position for the
	// camera will be [desiredFollowDistance] units away from [playerPosCameraMoveTarget]
	float desiredFollowDistance = 25;

	// create copy of [cameraToPlayerMove] and scale it a magnitude of [desiredFollowDistance]
	Vector3 newCameraToPlayer = cameraToPlayerMove;
	newCameraToPlayer.Normalize();
	newCameraToPlayer.Scale(desiredFollowDistance);

	// invert [newCameraToPlayer] because it will be added to [playerPosCameraMoveTarget] to form
	// the camera's real target position
	newCameraToPlayer.Invert();

	// calculate camera's real target position, [realCameraTargetPos], which will be [desiredFollowDistance] units
	// away from [playerPosCameraMoveTarget] along the vector [newCameraToPlayer]
	Point3 realCameraTargetPos;
	Point3::Add(playerPosCameraMoveTarget, newCameraToPlayer, realCameraTargetPos);

	// lerp the camera's current position towards its real target position
	Point3 newCameraPos;
	Point3::Lerp(cameraPos, realCameraTargetPos, newCameraPos, 2 * Time::GetDeltaTime());

	// get a vector that represents the lerp operation above
	Vector3 cameraMove;
	Point3::Subtract(newCameraPos, cameraPos, cameraMove);

	// get quaternion that represents a rotation from the camera's original forward vector
	// to [cameraToPlayerLookA], then from [cameraToPlayerLookA] to [cameraToPlayerLookB]
	// to form the final camera rotation [cameraRotation]
	Quaternion cameraRotationXZ;
	Quaternion cameraRotationY;
	Quaternion cameraRotation;
	cameraRotationXZ = Quaternion::getRotation(baseCameraForward,cameraToPlayerLookA);
	cameraRotationY = Quaternion::getRotation(cameraToPlayerLookA,cameraToPlayerLookB);
	cameraRotationXZ.normalize();
	cameraRotationY.normalize();
	cameraRotation = cameraRotationY * cameraRotationXZ;

	Quaternion currentRotation;
	Vector3 currentTranslation;
	Vector3 currentScale;

	// apply quaternion calculated above to make the camera look towards the player
	cameraObject->GetTransform().GetLocalComponents(currentTranslation, currentRotation, currentScale);
	cameraObject->GetTransform().SetLocalComponents(currentTranslation, cameraRotation, currentScale);

	// move the camera's position along the lerp'd movement vector calculated earlier [cameraMove]
	cameraObject->GetTransform().Translate(cameraMove, false);
}

/*
 * Set the player's current state.
 */
void Game::ActivatePlayerState(PlayerState state)
{
	playerState = state;
	stateActivationTime[(int) state] = Time::GetRealTimeSinceStartup();
}

