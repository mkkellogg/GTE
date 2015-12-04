/*
 * Class: Game
 *
 * Author: Mark Kellogg
 *
 * This is a basic demo to show some basic features of the GTE engine.
 */

#ifndef _GTE_GAME_H_
#define _GTE_GAME_H_

#include <functional>
#include <vector>

#include "engine.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "asset/assetimporter.h"
#include "scene.h"
#include "base/binarymask.h"

class Game
{
	enum class PlayerType
	{
		Koopa = 0,
		Warrior = 1
	};

	enum PlayerState
	{
		Waiting = 0,
		Walking = 1,
		Roaring = 2,
		JumpStart = 3,
		Jump = 4,
		JumpFall = 5,
		JumpEnd = 6,
		Attack1 = 7,
		Attack2 = 8,
		Attack3 = 9,
		Defend1 = 10,
		_Count = 11
	};

	enum class SceneLighting
	{
		None = 0,
		Ambient = 1,
		Directional = 2,
		Point = 3,
		Lava = 4
	};

	enum class Scenes
	{
		LavaScene = 0,
		CastleScene = 1,
		PoolScene = 2
	};

	// layer name for player object
	static const std::string PlayerObjectLayer;
	// layer mask for player object
	GTE::IntMask playerObjectLayerMask;

	static const GTE::UInt32 MaxPlayerStates = 32;

	// time at which a state was most recently activated
	GTE::Real stateActivationTime[MaxPlayerStates];

	// player's current state
	PlayerState playerState;
	// specify which model to load for player object
	PlayerType playerType;


	// SceneObject that contains the main camera for the scene
	GTE::SceneObjectSharedPtr cameraObject;
	// The SceneObject to which the player model hierarchy is attached
	GTE::SceneObjectSharedPtr playerObject;

	// scene object that holds the scene's directional light
	GTE::SceneObjectSharedPtr directionalLightObject;
	// scene object that holds the scene's ambient light
	GTE::SceneObjectSharedPtr ambientLightObject;
	//The SkinnedMesh3DRenderer that render the player's skinned mesh
	GTE::SkinnedMesh3DRendererSharedPtr playerRenderer;
	// player animations
	GTE::AnimationSharedPtr playerAnimations[PlayerState::_Count];
	// The AnimationPlayer responsible for managing the animations of the player 
	GTE::AnimationPlayerSharedPtr animationPlayer;

	// number of frames rendered;
	GTE::UInt32 frameCount;
	// should we print out the graphics engine FPS?
	GTE::Bool printFPS;
	// last time FPS was retrieved from the graphics engine
	GTE::Real lastFPSRetrieveTime;
	// last fps value retrieved from the graphics engine
	GTE::Real lastFPS;
	// last time info was printed
	GTE::Real lastInfoPrintTime;

	// lighting type that currently can be modified by the user
	SceneLighting selectedLighting;
	// is the directional light set to 'on' by the user?
	GTE::Bool directionalLightOn;

	class SceneTransition
	{
	public:

		GTE::Transform OriginalTransform;
	};

	// total number of scenes
	static const GTE::Int32 SceneCount = 3;
	// scene transition descriptors
	SceneTransition sceneTransitions[SceneCount];
	// scenes
	Scene* scenes[SceneCount];
	// current scene index
	Scenes currentScene;
	// are we transitioning between scenes?
	GTE::Bool sceneTransitioning;
	// when did the current scene transition start?
	GTE::Real sceneTransitionStartTime;
	// scene we are transitioning from
	Scenes sceneTransitionSrc;
	// scene we are transitioning to
	Scenes sceneTransitionDest;

	// movement variables
	GTE::Real playerWalkSpeed;
	GTE::Real playerRunSpeed;
	GTE::Real playerRotateSpeed;
	GTE::Real playerSpeedSmoothing;
	GTE::Real playerHorizontalSpeed;
	GTE::Real playerBaseY;
	GTE::Real playerVerticalSpeed;
	GTE::Bool playerIsMoving;
	GTE::Bool playerIsGrounded;

	// booleans that are only true during the frame they become true
	GTE::Bool playerJumpApexReached;
	GTE::Bool playerLanded;
	GTE::Bool displayInfoChanged;

	// player direction vectors
	GTE::Vector3 playerMoveDirection;
	GTE::Vector3 playerLookDirection;

	// default direction vectors
	GTE::Vector3 basePlayerForward;
	GTE::Vector3 baseCameraForward;

	void SetupScenes(GTE::AssetImporter& importer);
	void SetupScene(GTE::AssetImporter& importer, Scenes scene);
	void SetupGlobalElements(GTE::AssetImporter& importer);

	void SwitchToScene(Scenes scene);
	void TransitionToScene(Scenes scene);
	void UpdateSceneTransition();
	void SetupTransitionForScene(Scenes scene);

	void SetupCamera();
	void SetupPlayer(GTE::AssetImporter& importer);

	void InitializePlayerPosition();
	void UpdatePlayerHorizontalSpeedAndDirection();
	void UpdatePlayerVerticalSpeed();
	void UpdatePlayerAnimation();
	void ApplyPlayerMovement();
	void UpdatePlayerLookDirection();
	void UpdatePlayerFollowCamera();
	void ActivatePlayerState(PlayerState state);
	void ManagePlayerState();
	void HandleGeneralInput();
	void UpdateLight(GTE::SceneObjectSharedPtr sceneObject, GTE::Bool toggleLight, GTE::Real intensityChange, GTE::Bool toggleCastShadows);

	void DisplayInfo();
	void SignalDisplayInfoChanged();

public:

	Game();
	~Game();

	void Init();
	void Update();
	void OnPreRender();
	void OnQuit();
};

#endif
