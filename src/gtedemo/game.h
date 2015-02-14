/*
 * Class: Game
 *
 * Author: Mark Kellogg
 *
 * This is a basic demo to show some basic features of the GTE engine.
 */

#ifndef _GTE_GAME_H_
#define _GTE_GAME_H_

//forward declarations
class AssetImporter;

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include <functional>

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

	static const unsigned int MAX_PLAYER_STATES = 32;

	// time at which a state was most recently activated
	float stateActivationTime[MAX_PLAYER_STATES];

	// player's current state
	PlayerState playerState;

	// specify which model to load for player object
	PlayerType playerType;

	// SceneObject that contains the main camera for the scene
	SceneObjectRef cameraObject;
	// SceneObject that contains the single point light in the scene
	SceneObjectRef pointLightObject;
	// The SceneObject to which the player model hierarchy is attached
	SceneObjectRef playerObject;
	// The single cube in the scene
	SceneObjectRef cubeSceneObject;
	//The SkinnedMesh3DRenderer that render the player's skinned mesh
	SkinnedMesh3DRendererRef playerRenderer;
	// player animations
	AnimationRef playerAnimations[PlayerState::_Count];
	// The AnimationPlayer responsible for managing the animations of the player character
	AnimationPlayerRef animationPlayer;

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

	Vector3 playerMoveDirection;
	Vector3 playerLookDirection;
	Vector3 basePlayerForward;
	Vector3 baseCameraForward;

	void SetupCamera();
	void SetupScenery(AssetImporter& importer);
	void SetupPlayer(AssetImporter& importer);
	void SetupLights(AssetImporter& importer);

	void InitializePlayerPosition();
	void UpdatePlayerHorizontalSpeedAndDirection();
	void UpdatePlayerVerticalSpeed();
	void UpdatePlayerAnimation();
	void ApplyPlayerMovement();
	void UpdatePlayerLookDirection();
	void UpdatePlayerFollowCamera();
	void ActivatePlayerState(PlayerState state);
	void ManagePlayerState();

	void ProcessSceneObjects(SceneObjectRef ref, std::function<void(SceneObjectRef)> func);

    public:

    Game();
    ~Game();

    SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(SceneObjectRef ref);
    Mesh3DRef FindFirstMesh(SceneObjectRef ref);
    void Init();
    void Update();
};

#endif
