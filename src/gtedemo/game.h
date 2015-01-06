#ifndef _GAME_H_
#define _GAME_H_

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"

class Game
{
	enum class PlayerType
	{
		Koopa = 0,
		Nerd = 1
	};

	PlayerType playerType;

	SceneObjectRef cameraObject;
	SceneObjectRef pointLightObject;
	SceneObjectRef playerObject;
	SceneObjectRef cube;
	SkinnedMesh3DRendererRef playerRenderer;
	AnimationRef playerWait, playerWalk, playerJump, playerRoar;
	AnimationPlayerRef animationPlayer;

	float pointLightSegmentTime;
	int pointLightSegment;

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

	void InitializePlayerPosition();
	void UpdatePlayerMovementDirection();
	void UpdatePlayerAnimation();
	void UpdatePlayerPosition();
	void UpdatePlayerLookDirection();
	void UpdatePlayerFollowCamera();

    public:

    Game();
    ~Game();

    SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(SceneObjectRef ref);
    Mesh3DRef FindFirstMesh(SceneObjectRef ref);
    void Init();
    void Update();
};

#endif
