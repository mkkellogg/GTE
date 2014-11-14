#ifndef _GAME_H_
#define _GAME_H_

#include "object/enginetypes.h"

class Game
{
	float offset;
	SceneObjectRef cameraObject;
	SceneObjectRef koopaRoot;
	SkinnedMesh3DRendererRef koopaRenderer;
	AnimationRef koopaAnim;
	AnimationRef koopaWait, koopaWalk, koopaJump, koopaRoar;
	int boneIndex ;
	bool isWalking;
	bool isJumping;
	int rotationDir;

    public:

    Game();
    ~Game();

    SkinnedMesh3DRendererRef FindFirstSkinnedMeshRenderer(SceneObjectRef ref);
    void Init();
    void Update();
};

#endif
