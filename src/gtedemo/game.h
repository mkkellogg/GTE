/*
 * Class: Game
 *
 * Author: Mark Kellogg
 *
 * This is a basic demo to show some basic features of the GTE engine.
 */

#ifndef _GTE_GAME_H_
#define _GTE_GAME_H_

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "geometry/transform.h"
#include "asset/assetimporter.h"
#include "scene.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

using namespace GTE;

class Game
{
	enum class PlayerType
	{
		Koopa = 0,
		Warrior=1
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
	IntMask playerObjectLayerMask;

	static const unsigned int MaxPlayerStates = 32;

	// time at which a state was most recently activated
	float stateActivationTime[MaxPlayerStates];

	// player's current state
	PlayerState playerState;
	// specify which model to load for player object
	PlayerType playerType;


	// SceneObject that contains the main camera for the scene
	SceneObjectRef cameraObject;
	// The SceneObject to which the player model hierarchy is attached
	SceneObjectRef playerObject;

	// scene object that holds the scene's directional light
	SceneObjectRef directionalLightObject;
	// scene object that holds the scene's ambient light
	SceneObjectRef ambientLightObject;
	//The SkinnedMesh3DRenderer that render the player's skinned mesh
	SkinnedMesh3DRendererRef playerRenderer;
	// player animations
	AnimationRef playerAnimations[PlayerState::_Count];
	// The AnimationPlayer responsible for managing the animations of the player character
	AnimationPlayerRef animationPlayer;

	// number of frames rendered;
	unsigned int frameCount;
	// should we print out the graphics engine FPS?
	bool printFPS;
	// last time FPS was retrieved from the graphics engine
	float lastFPSRetrieveTime;
	// last fps value retrieved from the graphics engine
	float lastFPS;
	// last time info was printed
	float lastInfoPrintTime;

	// lighting type that currently can be modified by the user
	SceneLighting selectedLighting;

	class SceneTransition
	{
		public:

		Transform OriginalTransform;
	};

	// total number of scenes
	static const int SceneCount = 3;
	// scene transition descriptors
	SceneTransition sceneTransitions[SceneCount];
	// scenes
	Scene* scenes[SceneCount];
	// current scene index
	Scenes currentScene;
	// are we transitioning between scenes?
	bool sceneTransitioning;
	// when did the current scene transition start?
	float sceneTransitionStartTime;
	// scene we are transitioning from
	Scenes sceneTransitionSrc;
	// scene we are transitioning to
	Scenes sceneTransitionDest;

	// movement variables
	float playerWalkSpeed;
	float playerRunSpeed;
	float playerRotateSpeed;
	float playerSpeedSmoothing;
	float playerHorizontalSpeed;
	float playerBaseY;
	float playerVerticalSpeed;
	bool playerIsMoving;
	bool playerIsGrounded;

	// booleans that are only true during the frame they become true
	bool playerJumpApexReached;
	bool playerLanded;
	bool displayInfoChanged;

	// player direction vectors
	Vector3 playerMoveDirection;
	Vector3 playerLookDirection;

	// default direction vectors
	Vector3 basePlayerForward;
	Vector3 baseCameraForward;

	void SetupScenes(AssetImporter& importer);
	void SetupScene(AssetImporter& importer, Scenes scene);
	void SetupGlobalElements(AssetImporter& importer);

	void SwitchToScene(Scenes scene);
	void TransitionToScene(Scenes scene);
	void UpdateSceneTransition();
	void SetupTransitionForScene(Scenes scene);

	void SetupCamera();
	void SetupPlayer(AssetImporter& importer);

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
	void UpdateLight(SceneObjectRef sceneObject, bool toggleLight, float intensityChange, bool toggleCastShadows);

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
