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
		Nerd = 1
	};

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
	AnimationRef playerWait, playerWalk, playerJump, playerRoar;
	// The AnimationPlayer responsible for managing the animations of the player character
	AnimationPlayerRef animationPlayer;

	// movement variables
	float walkAnimationSpeed;
	float walkSpeed;
	float runAnimationSpeed;
	float runSpeed;
	float rotateSpeed;
	float speedSmoothing;
	float moveSpeed;
	bool isMoving;

	Vector3 moveDirection;
	Vector3 lookDirection;
	Vector3 basePlayerForward;
	Vector3 baseCameraForward;

	bool isGrounded;

	void SetupCamera();
	void SetupScenery(AssetImporter& importer);
	void SetupPlayer(AssetImporter& importer);
	void SetupLights(AssetImporter& importer);

	void InitializePlayerPosition();
	void UpdatePlayerMovementDirection();
	void UpdatePlayerAnimation();
	void UpdatePlayerPosition();
	void UpdatePlayerLookDirection();
	void UpdatePlayerFollowCamera();

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
