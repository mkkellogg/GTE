/*********************************************
*
* class: AnimationInstance
*
* author: Mark Kellogg
*
* This class encapsulates the state of a single instance
* of the playback an Animation object.
*
***********************************************/

#ifndef _ANIMATION_INSTANCE_H_
#define _ANIMATION_INSTANCE_H_

#include "object/enginetypes.h"
#include <vector>
#include <string>

class AnimationInstance
{
	friend class EngineObjectManager;
	friend class AnimationManager;
	friend class AnimationPlayer;

	public:

	/*
	 * The FrameState class is used to keep track of the state of the animation for
	 * each node in the target Skeleton [Target]. The Animation object has a member
	 * array [FrameStates] which holds the instance for each node.
	 */
	class FrameState
	{
		public:

		// index of the current translation key for the node
		unsigned int TranslationKeyIndex;
		// index of the current scale key for the node
		unsigned int ScaleKeyIndex;
		// index of the current rotation key for the node
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

	// the target of this instance
	SkeletonRef Target;
	// the Animation for which this is an instance
	AnimationRef SourceAnimation;

	// number of states in [FrameStates]. This number should equal the number of nodes in [target].
	// it should also be equal to the number of KeyFrameSet objects in [SourceAnimation].
	unsigned int StateCount;
	// array of FrameState objects, one for each node in [target] and is indexed in the same way.
	// E.g. The FrameState at index 5 corresponds to the SkeletonNode returned by target->GetNode(5);
	FrameState * FrameStates;

	// duration of this instance in seconds
	float Duration;
	// current progress (in seconds) of this instance
	float Progress;

	// duration of this instance in ticks
	float DurationTicks;
	// current progress (in ticks) of this instance
	float ProgressTicks;

	// is the animation playing?
	bool Playing;

	AnimationInstance(SkeletonRef target, AnimationRef animation);
	 ~AnimationInstance();

	void Destroy();

    void Play();
    void Stop();
    void Pause();

	public:

    bool Init();
    void Reset();

    FrameState * GetFrameState(unsigned int stateIndex);

    bool IsPlaying();
};

#endif
