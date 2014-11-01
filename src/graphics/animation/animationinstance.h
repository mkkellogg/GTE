#ifndef _ANIMATION_INSTANCE_H_
#define _ANIMATION_INSTANCE_H_

//forward declarations

#include "object/enginetypes.h"
#include <vector>
#include <string>

class AnimationInstance
{
	public:

	class FrameState
	{
		public:

		unsigned int TranslationKeyIndex;
		unsigned int ScaleKeyIndex;
		unsigned int RotationKeyIndex;

		FrameState()
		{
			TranslationKeyIndex = 0;
			ScaleKeyIndex = 0;
			RotationKeyIndex = 0;
		}
	};

	private:

	SkeletonRef target;
	AnimationRef animation;

	unsigned int stateCount;
	FrameState * frameStates;

	float duration;
	float startTime;
	float lastTime;

	void Destroy();

	public:

	AnimationInstance(SkeletonRef target, AnimationRef animation);
    ~AnimationInstance();

    bool Init();
    FrameState * GetFrameState(unsigned int nodeIndex);
};

#endif
