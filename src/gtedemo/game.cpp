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
#include "scenes/lavascene.h"
#include "scenes/castlescene.h"
#include "scenes/poolscene.h"
#include "asset/assetimporter.h"
#include "graphics/graphics.h"
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

	basePlayerForward = Vector3::Forward;
	basePlayerForward.Invert();
	baseCameraForward = Vector3::Forward;

	// initialize player state
	playerType = PlayerType::Warrior;
	playerState = PlayerState::Waiting;
	for(unsigned int i = 0; i < MaxPlayerStates; i++)
	{
		stateActivationTime[i] = 0;
	}
	playerObjectLayerMask = 0;

	// FPS variables
	printFPS = false;
	lastFPSRetrieveTime = 0;
	lastFPS = 0;
	lastInfoPrintTime = 0;
	displayInfoChanged = false;

	selectedLighting = SceneLighting::None;

	currentScene = Scenes::LavaScene;

	frameCount = 0;

	sceneTransitioning = false;
}

/*
 * Clean-up
 */
Game::~Game()
{

}

/*
 * Initialize the game. This method calls functions to set up the game camera, load & set up scenery,
 * load the player model & animations, and set up the scene lights.
 */
void Game::Init()
{
	// instantiate an asset importer to load models
	AssetImporter importer;

	// set up player layer mask
	LayerManager& layerManager = Engine::Instance()->GetEngineObjectManager()->GetLayerManager();
	int playerObjectLayerIndex = layerManager.AddLayer(PlayerObjectLayer);
	playerObjectLayerMask = layerManager.GetLayerMask(playerObjectLayerIndex);

	// set up global scene elements
	SetupGlobalElements(importer);

	// setup & initialize player
	SetupPlayer(importer);
	InitializePlayerPosition();

	SetupCamera();

	// set up the individual scenes
	SetupScenes(importer);
}

/*
 * Set up the individual scenes that showcase the various features of the
 * engine, and initialize the transitions between them.
 */
void Game::SetupScenes(AssetImporter& importer)
{
	SetupScene(importer, Scenes::LavaScene);
	SetupScene(importer, Scenes::CastleScene);
	SetupScene(importer, Scenes::PoolScene);

	SetupTransitionForScene(Scenes::LavaScene);
	SetupTransitionForScene(Scenes::CastleScene);
	SetupTransitionForScene(Scenes::PoolScene);
}

/*
 * Redirect to appropriate scene loading function base on [scene].
 */
void Game::SetupScene(AssetImporter& importer, Scenes scene)
{
	LavaScene * lavaScene = NULL;
	CastleScene * castleScene = NULL;
	PoolScene * poolScene = NULL;

	switch(scene)
	{
		case Scenes::LavaScene:
			lavaScene = new LavaScene();
			scenes[(unsigned int)Scenes::LavaScene] = lavaScene;
			lavaScene->Setup(importer, ambientLightObject, directionalLightObject, playerObject);
		break;
		case Scenes::CastleScene:
			castleScene = new CastleScene();
			scenes[(unsigned int)Scenes::CastleScene] = castleScene;
			castleScene->Setup(importer, ambientLightObject, directionalLightObject, playerObject);
		break;
		case Scenes::PoolScene:
			poolScene = new PoolScene();
			poolScene->SetMainCamera(cameraObject->GetCamera());
			scenes[(unsigned int)Scenes::PoolScene] = poolScene;
			poolScene->Setup(importer, ambientLightObject, directionalLightObject, playerObject);
		break;
	}
}

/*
 * Set up scene elements that are not specific not any particular scene.
 */
void Game::SetupGlobalElements(AssetImporter& importer)
{
	// get reference to the engine's object manager
	EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

	//========================================================
	//
	// Ambient light
	//
	//========================================================

	IntMask mergedMask;

	// create ambient light
	ambientLightObject = objectManager->CreateSceneObject();
	LightRef light = objectManager->CreateLight();
	light->SetIntensity(.30);
	light->SetType(LightType::Ambient);
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(light->GetCullingMask(), playerObjectLayerMask);
	light->SetCullingMask(mergedMask);
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
	mergedMask = objectManager->GetLayerManager().MergeLayerMask(light->GetCullingMask(), playerObjectLayerMask);
	light->SetCullingMask(mergedMask);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Directional);
	directionalLightObject->SetLight(light);

	importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);

	//========================================================
	//
	// start island
	//
	//========================================================

	// load castle island model
	SceneObjectRef modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load island model!\n");
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// place island in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.03,.03,.03, false);
	modelSceneObject->GetTransform().Translate(45,-10, 50, false);

	//========================================================
	//
	// wood bridge
	//
	//========================================================

	// load bridge
	modelSceneObject = importer.LoadModelDirect("resources/models/bridge/bridge.fbx", 1 , false, true);
	ASSERT(modelSceneObject.IsValid(), "Could not load bridge model!\n");
	GameUtil::SetAllObjectsStatic(modelSceneObject);

	// place bridge in the scene
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Scale(.1,.1,.1, false);
	modelSceneObject->GetTransform().Translate(30,-11.5,35,false);
	modelSceneObject->GetTransform().Rotate(0,1,0,55,true);
}

/*
 * Activate all scene objects in [scene] and deactivate all others.
 */
void Game::SwitchToScene(Scenes scene)
{
	currentScene = scene;

	for(unsigned int sceneIndex = 0; sceneIndex < SceneCount; sceneIndex++)
	{
		if(currentScene != (Scenes)sceneIndex)
		{
			scenes[sceneIndex]->GetSceneRoot()->SetActive(false);
		}
	}

	scenes[(unsigned int)currentScene]->GetSceneRoot()->SetActive(true);

	//Engine::Instance()->GetRenderManager()->ClearCaches();
}

/*
 * Activate a transition from the current scene to [scene]. The transition
 * will take place over several frames.
 */
void Game::TransitionToScene(Scenes scene)
{
	// don't start a transition into the current scene
	if(scene == currentScene)return;

	// initialize transition only if one is not in progress
	if(!sceneTransitioning)
	{
		sceneTransitionSrc = currentScene;
		sceneTransitionDest = scene;
		sceneTransitionStartTime = Time::GetRealTimeSinceStartup();
		sceneTransitioning = true;
		currentScene = scene;
		scenes[(unsigned int)scene]->OnActivate();
		SignalDisplayInfoChanged();
	}
}

/*
 * This method is called once per frame and is responsible for advancing
 * the progress of a scene transition.
 */
void Game::UpdateSceneTransition()
{
	if(sceneTransitioning)
	{
		// total time transition should last
		float transitionTime = .25;
		// full elapsed time
		float elapsedTime = Time::GetRealTimeSinceStartup() - sceneTransitionStartTime;
		// elapsed time scaled to the range 0..1
		float normalizedElapsedTime = elapsedTime / transitionTime;

		// from scene ...
		Scene* srcSceneObj = scenes[(unsigned int)sceneTransitionSrc];
		// ... to scene
		Scene* destSceneObj = scenes[(unsigned int)sceneTransitionDest];

		// source scene root
		SceneObjectRef srcRoot = srcSceneObj->GetSceneRoot();
		// destination scene root
		SceneObjectRef destRoot = destSceneObj->GetSceneRoot();

		// activate destination scene
		destRoot->SetActive(true);
		// deactivate source scene
		srcRoot->SetActive(false);

		float destScale = 1;

		// the transition has 4 phases, each representing a scale value
		unsigned int phases = 4;
		float times[] = {0, .6, .8, 1};
		float scales[] = {0, 1.2, .9, 1};

		// find where the elapsed time currently lies relative to the phase
		// boundaries, and interpolate the scale between the current phase
		// and the next phase based on elapsed time
		for(unsigned int i = 1; i < phases; i++)
		{
			if(normalizedElapsedTime < times[i])
			{
				float cElapsed = normalizedElapsedTime - times[i-1];
				destScale =  ((cElapsed/(times[i]-times[i-1])) * (scales[i] - scales[i-1])) + scales[i-1];
				break;
			}
		}

		if(elapsedTime > transitionTime)destScale = 1;

		SceneTransition& destTransition = sceneTransitions[(unsigned int)sceneTransitionDest];

		// apply calculated/interpolated scale
		Transform destTransform;
		destTransform.SetTo(destTransition.OriginalTransform);
		Vector3 destScaleVec(destScale,destScale,destScale);
		destTransform.Scale(destScaleVec, true);
		destRoot->GetTransform().SetTo(destTransform);

		// is the transition complete?
		if(elapsedTime > transitionTime)
		{
			SwitchToScene(sceneTransitionDest);
			sceneTransitioning = false;
		}
	}
}

/*
 * Initialize transition data for [scene].
 */
void Game::SetupTransitionForScene(Scenes scene)
{
	SceneTransition& sceneTransition = sceneTransitions[(unsigned int)scene];
	Scene* sceneObj = scenes[(unsigned int)scene];

	// for now we are only saving the original transform of the scene root
	SceneObjectRef sceneRoot = sceneObj->GetSceneRoot();
	sceneTransition.OriginalTransform.SetTo(sceneRoot->GetTransform());
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
	camera->SetRenderOrderIndex(5);
	camera->SetupCopyRenderTarget();
	cameraObject->SetCamera(camera);

	// specify which kinds of render buffers to use for this camera
	camera->AddClearBuffer(RenderBufferType::Color);
	camera->AddClearBuffer(RenderBufferType::Depth);

	camera->SetSSAOEnabled(true);

	// move camera object to its initial position
	Vector3 trans;
	Vector3 scale;
	Quaternion rot;
	Matrix4x4 mat;

	// decompose player's transform into position, rotation and scale
	playerObject->GetTransform().CopyMatrix(mat);
	mat.Decompose(trans,rot,scale);
	cameraObject->GetTransform().Translate(trans.x+20,trans.y+10,trans.z+15, true);

	// create skybox texture
	TextureRef skyboxTexture = objectManager->CreateCubeTexture("resources/textures/skybox-night/nightsky_north.png",
														 	 	"resources/textures/skybox-night/nightsky_south.png",
														 	 	"resources/textures/skybox-night/nightsky_up.png",
														 	 	"resources/textures/skybox-night/nightsky_down.png",
														 	 	"resources/textures/skybox-night/nightsky_west.png",
														 	 	"resources/textures/skybox-night/nightsky_east.png");
	// activate skybox
	camera->SetupSkybox(skyboxTexture);
	camera->SetSkyboxEnabled(true);
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
			ASSERT(playerObject.IsValid(), "Could not load Koopa model!\n");
			playerObject->GetTransform().SetIdentity();
			playerObject->GetTransform().Translate(45,-10,55,false);
			playerObject->GetTransform().Scale(.05, .05, .05, true);
		break;
		case PlayerType::Warrior:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, true);
			playerObject = importer.LoadModelDirect("resources/models/toonwarrior/character/warrior.fbx");
			ASSERT(playerObject.IsValid(), "Could not load Warrior model!\n");
			playerObject->GetTransform().Translate(45,-10,55,false);
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
	GameUtil::SetAllObjectsLayerMask(playerObject, playerObjectLayerMask);
	playerRenderer = GameUtil::FindFirstSkinnedMeshRenderer(playerObject);
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

			ASSERT(compatible, "Koopa animations are not compatible!");

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

			ASSERT(compatible, "Warrior animations are not compatible!");

			// set all meshes to use standard shadow volume
			GameUtil::ProcessSceneObjects(playerObject, [=](SceneObjectRef current)
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
	UpdatePlayerHorizontalSpeedAndDirection();
	UpdatePlayerVerticalSpeed();
	ApplyPlayerMovement();
	UpdatePlayerAnimation();
	UpdatePlayerLookDirection();
	UpdatePlayerFollowCamera();
	ManagePlayerState();
	HandleGeneralInput();

	if(!sceneTransitioning)
	{
		scenes[(unsigned int)currentScene]->Update();
	}
	UpdateSceneTransition();

	DisplayInfo();

	frameCount++;
}

/*
 * OnPreRender() is called once per frame, after pre-processing is done on scene, and
 * after the Update() call.
 */
void Game::OnPreRender()
{
	if(frameCount == 1)
	{
		SwitchToScene(Scenes::LavaScene);
	}
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
			lastFPS = Engine::Instance()->GetGraphicsSystem()->GetCurrentFPS();
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

		switch(currentScene)
		{
			case Scenes::LavaScene:
				printf(" |  Current scene: Lava ");
			break;
			case Scenes::CastleScene:
				printf(" |  Current scene: Castle");
			break;
			case Scenes::PoolScene:
				printf(" |  Current scene: Reflecting Pool");
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
		CameraRef mainCamera = cameraObject->GetCamera();
		mainCamera->SetSSAOEnabled(!mainCamera->IsSSAOEnabled());
	}

	// toggle ssao render mode
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::I))
	{
		CameraRef mainCamera = cameraObject->GetCamera();
		if(mainCamera->GetSSAORenderMode() == SSAORenderMode::Outline)mainCamera->SetSSAORenderMode(SSAORenderMode::Standard);
		else mainCamera->SetSSAORenderMode(SSAORenderMode::Outline);
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

	// get scene object pointers
	LavaScene *lavaScene = ((LavaScene*)scenes[(unsigned int)Scenes::LavaScene]);
	CastleScene *castleScene = ((CastleScene*)scenes[(unsigned int)Scenes::CastleScene]);
	PoolScene *poolScene = ((PoolScene*)scenes[(unsigned int)Scenes::PoolScene]);
	LavaField *lavaField = lavaScene->GetLavaField();

	// toggle lava
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::K))
	{
		SceneObjectRef lavaFieldObject = lavaField->GetSceneObject();
		lavaFieldObject->SetActive(!lavaFieldObject->IsActive());
	}

	// determine light actions based on key input
	bool toggleCastShadows = Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::R);
	bool boostLightIntensity = Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::W);
	bool reduceLightIntensity =  Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::E);
	bool toggleLight = Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::Q);

	float intensityBoost = 0;
	if(boostLightIntensity)intensityBoost = .05;
	else if(reduceLightIntensity)intensityBoost = -.05;

	// get references to various lights across multiple scenes
	std::vector<SceneObjectRef>& lavaLightObjects = lavaScene->GetLavaLightObjects();
	std::vector<SceneObjectRef>& castleLights = castleScene->GetPointLights();
	SceneObjectRef lavaSpinningLight = lavaScene->GetSpinningPointLightObject();
	std::vector<SceneObjectRef>& reflectingPoolLights = poolScene->GetPointLights();

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
			UpdateLight(lavaSpinningLight, toggleLight, intensityBoost, toggleCastShadows);
			for(unsigned int i =0; i < castleLights.size(); i++)
			{
				UpdateLight(castleLights[i], toggleLight, intensityBoost, toggleCastShadows);
			}
			for(unsigned int i =0; i < reflectingPoolLights.size(); i++)
			{
				UpdateLight(reflectingPoolLights[i], toggleLight, intensityBoost, toggleCastShadows);
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

	// change to lava scene
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::One))
	{
		TransitionToScene(Scenes::LavaScene);
	}

	// change to castle scene
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::Two))
	{
		TransitionToScene(Scenes::CastleScene);
	}

	// change to pool scene
	if(Engine::Instance()->GetInputManager()->ShouldHandleOnKeyDown(Key::Three))
	{
		TransitionToScene(Scenes::PoolScene);
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

