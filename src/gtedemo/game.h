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
class Vector3;
class Quaternion;
class LavaField;

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "base/intmask.h"
#include <functional>
#include <vector>

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

	// layer name for lava pool wall
	static const std::string LavaWallLayer;
	// layer name for lava pool island
	static const std::string LavaIslandLayer;
	// layer name for lava pool island objects
	static const std::string LavaIslandObjectsLayer;
	// layer name for player object
	static const std::string PlayerObjectLayer;
	// layer mask for lava wall layer
	IntMask lavaWallLayerMask;
	// layer mask for lava island layer
	IntMask lavaIslandLayerMask;
	// layer mask for lava island layer objects
	IntMask lavaIslandObjectsLayerMask;
	// layer mask for player object
	IntMask playerObjectLayerMask;

	static const unsigned int MAX_PLAYER_STATES = 32;

	// time at which a state was most recently activated
	float stateActivationTime[MAX_PLAYER_STATES];

	// player's current state
	PlayerState playerState;

	// specify which model to load for player object
	PlayerType playerType;

	// SceneObject that contains the main camera for the scene
	SceneObjectRef cameraObject;
	// SceneObject that contains the spinning point light in the scene
	SceneObjectRef spinningPointLightObject;
	// scene lava
	LavaField * lavaField;
	// container lava lights
	std::vector<SceneObjectRef> lavaLightObjects;
	// container for other point lights in the scene
	std::vector<SceneObjectRef> otherPointLightObjects;
	// The SceneObject to which the player model hierarchy is attached
	SceneObjectRef playerObject;
	// The single cube in the scene
	SceneObjectRef cubeSceneObject;
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

	void ProcessSceneObjects(SceneObjectRef ref, std::function<void(SceneObjectRef)> func);
	SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(SceneObjectRef ref);
	SceneObjectRef FindFirstSceneObjectWithMesh(SceneObjectRef ref);
	void SetAllObjectsStatic(SceneObjectRef root);
	void SetAllObjectsLayerMask(SceneObjectRef root, IntMask mask);
	void SetAllMeshesStandardShadowVolume(SceneObjectRef root);
	void SetAllObjectsCastShadows(SceneObjectRef root, bool castShadows);

	void SetupScene(AssetImporter& importer);
	void SetupSceneTerrain(AssetImporter& importer);
	void SetupSceneStructures(AssetImporter& importer);
	void SetupScenePlants(AssetImporter& importer);
	void SetupSceneExtra(AssetImporter& importer);
	SceneObjectRef AddMeshToScene(Mesh3DRef mesh, MaterialRef material, float sx, float sy, float sz, float rx, float ry, float rz, float ra, float tx, float ty, float tz,
								  bool isStatic, bool castShadows, bool receiveShadows);
	void SetupCamera();
	void SetupLights(AssetImporter& importer);
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
};

#endif
