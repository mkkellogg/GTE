#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <memory>
#include <functional>
#include "game.h"
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
#include "util/time.h"
#include "util/engineutility.h"
#include "global/global.h"
#include "global/constants.h"
#include "gtemath/gtemath.h"
#include "filesys/filesystem.h"

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
	lastFPSPrintTime = 0;
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
 * Initialize the game. This method calls functions to set up the game camera, load & set up scenery,
 * load the player model & animations, and set up the scene lights.
 */
void Game::Init()
{
	// instantiate an asset importer to load models
	AssetImporter importer;

	// set up the scene
	SetupScene(importer);
	SetupLights(importer);

	// setup & initialize player
	SetupPlayer(importer);
	InitializePlayerPosition();

	SetupCamera();
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

	// decompose camera's transform into position, rotation and scale
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
 * Set up the "land" elements in the scene.
 */
void Game::SetupSceneTerrain(AssetImporter& importer)
{
	// multi-use reference
	SceneObjectRef modelSceneObject;

	//========================================================
	//
	// Load island models
	//
	//========================================================

	// load island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load island model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.07,.05,.07, false);
	modelSceneObject->GetTransform().Translate(0,-10,0,false);

	// load island model
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load island model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.07,.05,.07, false);
	modelSceneObject->GetTransform().Translate(80,-10,-10,false);

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
	// Load castle components
	//
	//========================================================

	// load turrent tower
	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tower model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// pplace turret tower in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.05,.05,.05, false);
	modelSceneObject->GetTransform().Translate(10,-10,-10,false);

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
	AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 89,-10,-15, true);
	AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 65,-10,6, true);
	AddMeshToScene(tower2Mesh, towerMaterial, .04,.04,.03, 1,0,0, -90, 89,-10,6, true);

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
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 78,-10,6.5, true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 86,-10,6.5, true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 70,-10,-15.5, true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 78,-10, -15.5, true);
	AddMeshToScene(wallBlockMesh, wallBlockMaterial, .06,.04,.05, 1,0,0, -90, 86,-10, -15.5, true);
	modelSceneObject = AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.067,.05, 1,0,0, -90, 90,-10, -9.25, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);
	modelSceneObject = AddMeshToScene(wallBlockMesh, wallBlockMaterial, .04,.067,.05, 1,0,0, -90, 90,-10, .25, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,90,true);


	//========================================================
	//
	// Load mushroom house model
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/mushroom/MushRoom_01.fbx");
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load mushroom house model!\n");
	SetAllObjectsStatic(modelSceneObject);

	// place mushroom house in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.09,.09,.09, false);
	modelSceneObject->GetTransform().Translate(-10,-10,20,false);

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
	// Load trees
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
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .10,.10,.10, 1,0,0, -85, 57, -10, 24, true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .15,.15,.20, 1,0,0, -94, 61, -9, -15, true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .20,.20,.30, 1,0,0, -93, 80, -9, -15, true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .17,.17,.20, 1,0,0, -85, 85, -9.5, -13, true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .22,.22,.38, 1,0,0, -90, 115, -9.5, 15, true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .19,.19,.28, 1,0,0, -96, 105, -9.5, 8, true);
	SetAllMeshesStandardShadowVolume(modelSceneObject);
	modelSceneObject = AddMeshToScene(treeMesh, treeMaterial, .18,.18,.20, 1,0,0, -87, 95, -10, 32, true);
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
	// Load textured scene cube
	//
	//========================================================

	// create instance of SceneObject to hold the cube mesh and its renderer
	cubeSceneObject = objectManager->CreateSceneObject();

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
	cubeMesh->SetCastShadows(true);
	cubeMesh->SetReceiveShadows(true);
	Mesh3DFilterRef filter = objectManager->CreateMesh3DFilter();
	cubeSceneObject->SetMesh3DFilter(filter);
	filter->SetMesh3D(cubeMesh);

	// create the cube mesh's renderer
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetMesh3DRenderer(renderer);

	// scale the cube and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(1.5, 1.5,1.5, false);
	cubeSceneObject->GetTransform().Translate(2, -7, 10, false);

	//========================================================
	//
	// Load fences
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
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 47,-10,14.5, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-70,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 52,-10,20, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-25,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 69.5,-10,21, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,15,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 79.2,-10,19.3, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,5,true);

	// fence on left side of castle
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 51,-10,-16, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-120,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 55,-10,-25, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-110,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 59.9,-10,-33.3, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-135,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 68.2,-10,-38, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-160,true);
	modelSceneObject = AddMeshToScene(fenceMesh, fenceMaterial, .6,.6,.6, 1,0,0, -90, 78,-10,-40, true);
	modelSceneObject->GetTransform().Rotate(0,0,1,-175,true);


	//========================================================
	//
	// Load barrels
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
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .8,.8,.8, 1,0,0, -90, 74,-10,10.5, true);
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-8.3,1.5, true);
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-8.3,-1.8, true);
	modelSceneObject = AddMeshToScene(barrelMesh, barrelMaterial, .9,.9,.9, 0,1,0, 90, 92,-5.3,-.15, true);
}

/*
 * Redirects to the main AddMeshToScene(), passing in false for [isStatic].
 *
 */
SceneObjectRef Game::AddMeshToScene(Mesh3DRef mesh, MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra, float tx, float ty, float tz)
{
	return AddMeshToScene(mesh, material, sx, sy, sz, rx, ry, rz, ra, tx, ty, tz, false);
}

/*
 * Given an instance of Mesh3D [mesh] and and instance of Material [material], create an instance of SceneObject that
 * contains [mesh] and renders it using [material]. Scale the scene object's transform by [sx], [sy], [sz] in world
 * space, then rotate by [ra] degrees around the world space axis [rx], [ry], [rz], and then translate in world space by
 * [tx], [ty], tz]. If [isStatic] == true, then set the root SceneObject instance and all children to be static.
 *
 * This method is used to handle all the details of placing an arbitrary mesh somewhere in the scene at a specified orientation.
 */
SceneObjectRef Game::AddMeshToScene(Mesh3DRef mesh, MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra, float tx, float ty, float tz, bool isStatic)
{
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();
	SceneObjectRef meshSceneObject = objectManager->CreateSceneObject();
	meshSceneObject->SetActive(true);
	Mesh3DFilterRef meshFilter = objectManager->CreateMesh3DFilter();
	meshFilter->SetMesh3D(mesh);
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
 * Set up the player model and animations, use [importer] to load model files from disk.
 */
void Game::SetupPlayer(AssetImporter& importer)
{
	//========================================================
	//
	// Load player model
	//
	//========================================================

	SkinnedMesh3DRendererRef playerMeshRenderer;
	Mesh3DRef firstMesh;
	playerType = PlayerType::Warrior;
	playerState = PlayerState::Waiting;
	playerIsGrounded = true;

	// load player models
	switch(playerType)
	{
		case PlayerType::Koopa:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);
			playerObject = importer.LoadModelDirect("resources/models/koopa/koopamod.fbx");
			ASSERT_RTRN(playerObject.IsValid(), "Could not load Koopa model!\n");
			playerObject->SetActive(true);
			playerMeshRenderer = FindFirstSkinnedMeshRenderer(playerObject);
			playerObject->GetTransform().SetIdentity();
			playerObject->GetTransform().Translate(30,-10,-2,false);
			playerObject->GetTransform().Scale(.05, .05, .05, true);
		break;
		case PlayerType::Warrior:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, true);
			playerObject = importer.LoadModelDirect("resources/models/toonwarrior/character/warrior.fbx");
			ASSERT_RTRN(playerObject.IsValid(), "Could not load Warrior model!\n");
			playerObject->SetActive(true);
			playerMeshRenderer = FindFirstSkinnedMeshRenderer(playerObject);
			playerObject->GetTransform().Translate(50,-10,-10,false);
			playerObject->GetTransform().Scale(4, 4, 4, true);
		break;
	}

	// load player animations
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

	playerRenderer = FindFirstSkinnedMeshRenderer(playerObject);
	AnimationManager * animManager = Engine::Instance()->GetAnimationManager();
	bool compatible = true;

	// setup player animations
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
 * Set up the lights in the scene.
 */
void Game::SetupLights(AssetImporter& importer)
{
	SceneObjectRef sceneObject;

	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	// create self-illuminated cube mesh to represent spinning point light
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
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
	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(selflitMaterial);
	spinningPointLightObject->SetMesh3DRenderer(renderer);
	LightRef light = objectManager->CreateLight();
	light->SetIntensity(1.7);
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Point);
	spinningPointLightObject->SetLight(light);
	spinningPointLightObject->GetTransform().Scale(.4,.4,.4, true);
	spinningPointLightObject->GetTransform().Translate(5, 0, 20, false);

	// create ambient light
	ambientLightObject = objectManager->CreateSceneObject();
	light = objectManager->CreateLight();
	light->SetIntensity(.30);
	light->SetType(LightType::Ambient);
	ambientLightObject->SetLight(light);

	// create directional light
	directionalLightObject = objectManager->CreateSceneObject();
	directionalLightObject->SetStatic(true);
	light = objectManager->CreateLight();
	light->SetDirection(-.8,-1.7,-2);
	light->SetIntensity(.8);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Directional);
	directionalLightObject->SetLight(light);

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
	Point3 lightRotatePoint(10,5,18);

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

	if(printFPS)
	{
		float elapsedPrintTime = Time::GetRealTimeSinceStartup() - lastFPSPrintTime;
		if(elapsedPrintTime > 1)
		{
			printf("FPS: %f\r", Engine::Instance()->GetGraphicsEngine()->GetCurrentFPS());
			fflush(stdout);
			lastFPSPrintTime = Time::GetRealTimeSinceStartup();
		}
	}
	else
	{
		printf("                                            \r");
		fflush(stdout);
	}
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
	// toggle ambient lights
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::A))
	{
		if(ambientLightObject.IsValid())
		{
			ambientLightObject->SetActive(!ambientLightObject->IsActive());
		}
	}

	// toggle directional light
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::D))
	{
		if(directionalLightObject.IsValid())
		{
			directionalLightObject->SetActive(!directionalLightObject->IsActive());
		}
	}

	// toggle point lights
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::P))
	{
		if(spinningPointLightObject.IsValid())
		{
			spinningPointLightObject->SetActive(!spinningPointLightObject->IsActive());
		}

		for(unsigned int i =0; i < otherPointLightObjects.size(); i++)
		{
			SceneObjectRef otherPointLightObject = otherPointLightObjects[i];
			if(otherPointLightObject.IsValid())
			{
				otherPointLightObject->SetActive(!otherPointLightObject->IsActive());
			}
		}
	}

	// toggle skybox
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::S))
	{
		if(cameraObject.IsValid())
		{
			cameraObject->GetCamera()->SetSkyboxEnabled(!cameraObject->GetCamera()->IsSkyboxEnabled());
		}
	}

	// toggle printing of FPS
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::F))
	{
		printFPS = !printFPS;
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

void Game::ActivatePlayerState(PlayerState state)
{
	playerState = state;
	stateActivationTime[(int) state] = Time::GetRealTimeSinceStartup();
}

