#ifndef _ANIMATION_INSTANCE_H_
#define _ANIMATION_INSTANCE_H_

//forward declarations

#include "object/enginetypes.h"
#include <vector>
#include <string>

class AnimationInstance
{
	friend class EngineObjectManager;
	friend class AnimationManager;

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

		void Reset()
		{
			TranslationKeyIndex = 0;
			ScaleKeyIndex = 0;
			RotationKeyIndex = 0;
		}
	};

	private:

	SkeletonRef Target;
	AnimationRef Animation;

	unsigned int StateCount;
	FrameState * FrameStates;

	float Duration;
	float Progress;

	float DurationTicks;
	float ProgressTicks;

	float Weight;
	bool Playing;

	AnimationInstance(SkeletonRef target, AnimationRef animation);
	 ~AnimationInstance();
	void Destroy();

	public:

    bool Init();
    void Reset();

    FrameState * GetFrameState(unsigned int stateIndex);

    bool IsPlaying();
    void Play();
    void Stop();
    void Pause();
};

#endif
