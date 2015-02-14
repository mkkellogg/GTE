#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

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

	playerType = PlayerType::Koopa;
	playerState = PlayerState::Waiting;
	for(unsigned int i = 0; i < MAX_PLAYER_STATES; i++)
	{
		stateActivationTime[i] = 0;
	}
}

/*
 * Clean-up
 */
Game::~Game()
{

}

/*
 * Recursively search the scene hierarchy starting at [ref] for an instance of SceneObject that
 * contains a SkinnedMesh3DRenderer component.
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
 * Recursively search the scene hierarchy starting at [ref] for an instance of SceneObject that
 * contains a Mesh3D component.
 */
Mesh3DRef Game::FindFirstMesh(SceneObjectRef ref)
{
	if(!ref.IsValid())return Mesh3DRef::Null();

	if(ref->GetMesh3D().IsValid())return ref->GetMesh3D();

	for(unsigned int i = 0; i < ref->GetChildrenCount(); i++)
	{
		SceneObjectRef childRef = ref->GetChildAt(i);
		Mesh3DRef subRef = FindFirstMesh(childRef);
		if(subRef.IsValid())return subRef;
	}

	return Mesh3DRef::Null();
}

/*
 * Recursively visit objects in the scene hierarchy with root at [ref] and for each
 * invoke [func] with [ref] as the only parameter.
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

	SetupCamera();
	SetupScenery(importer);
	SetupLights(importer);

	SetupPlayer(importer);
	InitializePlayerPosition();
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
	cameraObject->GetTransform().Translate(0, 5, 25, true);
}

/*
 * Set up the scenery for the scene and use [importer] to load assets from disk.
 */
void Game::SetupScenery(AssetImporter& importer)
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
	Mesh3DRef firstMesh;
	StandardAttributeSet meshAttributes;
	SceneObjectRef childSceneObject;

	//========================================================
	//
	// Load cube
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

	// create the cube mesh and set its attributes
	meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
	cubeMesh = GameUtil::CreateCubeMesh(meshAttributes);
	cubeMesh->SetCastShadows(true);
	cubeMesh->SetReceiveShadows(true);
	cubeSceneObject->SetMesh3D(cubeMesh);

	// create the cube mesh's renderer
	renderer = objectManager->CreateMesh3DRenderer();
	renderer->AddMaterial(material);
	cubeSceneObject->SetMesh3DRenderer(renderer);

	// scale the cube and move to its position in the scene
	cubeSceneObject->GetTransform().Scale(1.5, 1.5,1.5, true);
	cubeSceneObject->GetTransform().Translate(2, -7, 10, false);


	//========================================================
	//
	// Load island model
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/island/island.fbx", 1 , false, true);
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load island model!\n");
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Translate(0,-10,0,false);
	modelSceneObject->GetTransform().Scale(.07,.05,.07, true);

	//========================================================
	//
	// Load tower model
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/castle/Tower_01.fbx", 1 );
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load tower model!\n");
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Translate(10,-10,-10,false);
	modelSceneObject->GetTransform().Scale(.05,.05,.05, true);

	//========================================================
	//
	// Load mushroom house model
	//
	//========================================================

	modelSceneObject = importer.LoadModelDirect("resources/models/toonlevel/mushroom/MushRoom_01.fbx", 1 );
	ASSERT_RTRN(modelSceneObject.IsValid(), "Could not load mushroom house model!\n");
	modelSceneObject->SetActive(true);
	modelSceneObject->GetTransform().Translate(-10,-10,20,false);
	modelSceneObject->GetTransform().Scale(.09,.09,.09, true);
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

	switch(playerType)
	{
		case PlayerType::Koopa:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, false);
			playerObject = importer.LoadModelDirect("resources/models/koopa/koopamod.fbx", 1 );
			ASSERT_RTRN(playerObject.IsValid(), "Could not load Koopa model!\n");
			playerObject->SetActive(true);
			playerMeshRenderer = FindFirstSkinnedMeshRenderer(playerObject);
			playerObject->GetTransform().Translate(0,-10,-2,false);
			playerObject->GetTransform().Scale(.05, .05, .05, true);
		break;
		case PlayerType::Warrior:
			importer.SetBoolProperty(AssetImporterBoolProperty::PreserveFBXPivots, true);
			playerObject = importer.LoadModelDirect("resources/models/toonwarrior/character/warrior.fbx", 1 );
			ASSERT_RTRN(playerObject.IsValid(), "Could not load Warrior model!\n");
			playerObject->SetActive(true);
			playerMeshRenderer = FindFirstSkinnedMeshRenderer(playerObject);
			playerObject->GetTransform().Translate(0,-10,-2,false);
			playerObject->GetTransform().Scale(5, 5, 5, true);
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

	playerRenderer = FindFirstSkinnedMeshRenderer(playerObject);
	AnimationManager * animManager = Engine::Instance()->GetAnimationManager();
	bool compatible = true;

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

			// set all warrior meshes to use standard shadow volume
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

	// create point light scene object
	pointLightObject = objectManager->CreateSceneObject();

	// create self-illuminated cube mesh to represent point light
	StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
	StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::VertexColor);
	Mesh3DRef cubeMesh = GameUtil::CreateCubeMesh(meshAttributes);

	// add the point light mesh to the point light scene object
	pointLightObject->SetMesh3D(cubeMesh);

	// create a renderer for the point light mesh
	Mesh3DRendererRef renderer = objectManager->CreateMesh3DRenderer();
	ShaderSource selfLitShaderSource;
	importer.LoadBuiltInShaderSource("selflit", selfLitShaderSource);
	MaterialRef selflitMaterial = objectManager->CreateMaterial("SelfLitMaterial", selfLitShaderSource);
	renderer->AddMaterial(selflitMaterial);

	// add renderer for the point light mesh to the point light scene object
	pointLightObject->SetMesh3DRenderer(renderer);

	// Create Light object and set its properties
	LightRef light = objectManager->CreateLight();
	light->SetDirection(1,-1,-1);
	light->SetIntensity(1.7);
	light->SetRange(25);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Point);

	// add Light object to the point light scene object
	pointLightObject->SetLight(light);

	// move point light scene object to its initial position in the scene
	pointLightObject->GetTransform().Scale(.4,.4,.4, true);
	pointLightObject->GetTransform().Translate(5, 0, 20, false);

	// create ambient light
	sceneObject = objectManager->CreateSceneObject();
	light = objectManager->CreateLight();
	light->SetIntensity(.30);
	light->SetType(LightType::Ambient);
	sceneObject->SetLight(light);

	// create directional light
	sceneObject = objectManager->CreateSceneObject();
	light = objectManager->CreateLight();
	light->SetDirection(-.8,-1.7,-2);
	light->SetIntensity(.8);
	light->SetShadowsEnabled(true);
	light->SetType(LightType::Directional);
	sceneObject->SetLight(light);
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
	pointLightObject->GetTransform().RotateAround(lightRotatePoint.x, lightRotatePoint.y, lightRotatePoint.z,0,1,0,60 * Time::GetDeltaTime(), false);
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
 * Update the camera's position and direction that it is facing.
 */
void Game::UpdatePlayerFollowCamera()
{
	Point3 cameraPos;
	Point3 playerPos;

	// point which the camera will be facing
	Point3 playerPosCameraLookTarget;
	// point to which the camera will move
	Point3 playerPosCameraMoveTarget;

	// get the gamera's position in world space into [cameraPos]
	cameraObject->GetTransform().TransformPoint(cameraPos);

	// get the player object's position in world space into [playerPos]
	playerObject->GetTransform().TransformPoint(playerPos);

	playerPosCameraLookTarget = playerPos;
	playerPosCameraMoveTarget = playerPos;

	// keep the y component of position at which the camera should look the same as the camera's current
	// position so that its orientation stays level in the x-z plane
	playerPosCameraLookTarget.y = cameraPos.y;

	// the target position to which the camera should move is 10 world units above the player object's position
	playerPosCameraMoveTarget.y = playerPos.y + 10;

	// get a vector from the camera's current position to the position at which it is looking,
	// and store in [cameraToPlayerLook]
	Vector3 cameraToPlayerLook;
	Point3::Subtract(playerPosCameraLookTarget, cameraPos, cameraToPlayerLook);

	// get a vector from the camera's current position to its target position,
	// and store in [cameraToPlayerMove]
	Vector3 cameraToPlayerMove;
	Point3::Subtract(playerPosCameraMoveTarget, cameraPos, cameraToPlayerMove);

	// project [cameraToPlayerMove] into the x-z plane
	cameraToPlayerMove.y=0;

	// target distance camera should be from the player object. this means the real target position for the
	// camera will be [desiredFollowDistance] units awya from [playerPosCameraMoveTarget]
	float desiredFollowDistance = 30;

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
	Point3::Lerp(cameraPos, realCameraTargetPos, newCameraPos, .4 * Time::GetDeltaTime());

	// get a vector that represents the lerp operation above
	Vector3 cameraMove;
	Point3::Subtract(newCameraPos, cameraPos, cameraMove);

	// get quaternion that represents a rotation from the camera's original forward vector
	// to [cameraToPlayerLook]
	Quaternion cameraRotationA;
	cameraRotationA = Quaternion::getRotation(baseCameraForward,cameraToPlayerLook);
	cameraRotationA.normalize();

	Quaternion currentRotation;
	Vector3 currentTranslation;
	Vector3 currentScale;

	// apply quaternion calculated above to make the camera look towards the player
	cameraObject->GetTransform().GetLocalComponents(currentTranslation, currentRotation, currentScale);
	cameraObject->GetTransform().SetLocalComponents(currentTranslation, cameraRotationA, currentScale);

	// move the camera's position along the lerp'd movement vector calculated earlier [cameraMove]
	cameraObject->GetTransform().Translate(cameraMove, false);
}

void Game::ActivatePlayerState(PlayerState state)
{
	playerState = state;
	stateActivationTime[(int) state] = Time::GetRealTimeSinceStartup();
}

