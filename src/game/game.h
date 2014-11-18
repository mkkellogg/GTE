#ifndef _GAME_H_
#define _GAME_H_

#include "object/enginetypes.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"

class Game
{
	SceneObjectRef cameraObject;
	SceneObjectRef playerObject;
	SkinnedMesh3DRendererRef koopaRenderer;
	AnimationRef koopaAnim;
	AnimationRef koopaWait, koopaWalk, koopaJump, koopaRoar;
	AnimationPlayerRef animationPlayer;

	float walkSpeed;
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
    void Init();
    void Update();
};

#endif
