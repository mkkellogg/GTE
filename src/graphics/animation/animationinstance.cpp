#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationinstance.h"
#include "object/enginetypes.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/skeleton.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

/*
 * Single constructor that initializes member variables.
 */
AnimationInstance::AnimationInstance(SkeletonRef target, AnimationRef animation)
{
	this->Target = target;
	this->SourceAnimation = animation;

	StateCount = 0;
	FrameStates = NULL;

	Duration = 0;
	Progress = 0;
	SpeedFactor = 1;

	DurationTicks = 0;
	ProgressTicks = 0;

	Playing = false;
	Paused = false;
}

/*
 * Default destructor
 */
AnimationInstance::~AnimationInstance()
{

}

/*
 * Destroy this animation instance. Delete all FrameState instances in [FrameStates].
 */
void AnimationInstance::Destroy()
{
	if(FrameStates != NULL)
	{
		delete[] FrameStates;
		FrameStates = NULL;
	}
	StateCount = 0;
}

/*
 * Initialize this instance. This method will allocate a FrameState object for each node in [Target].
 * It will also verify the the number of nodes in [Target] matches the number of KeyFrameSet objects
 * contained in [SourceAnimation].
 */
bool AnimationInstance::Init()
{
	ASSERT(Target.IsValid(), "AnimationInstance::Init -> Animation target is invalid.", false);
	ASSERT(SourceAnimation.IsValid(), "AnimationInstance::Init -> Animation is invalid.", false);

	Destroy();

	unsigned int nodeCount = Target->GetNodeCount();

	ASSERT(nodeCount == SourceAnimation->GetKeyFrameSetCount(),"AnimationInstance::Init -> Node count does not equal animation key frame set count.",false);

	if(nodeCount <= 0)return true;

	FrameStates = new FrameState[nodeCount];
	ASSERT(FrameStates != NULL, "AnimationInstance::Init -> Unable to allocate FrameState array.", false);

	for(unsigned int n = 0; n < nodeCount; n++)
	{
		FrameState state;
		FrameStates[n] = state;
	}
	StateCount = nodeCount;

	DurationTicks = SourceAnimation->GetDurationTicks();
	Duration = DurationTicks / SourceAnimation->GetTicksPerSecond();

	Progress = 0;
	ProgressTicks = 0L;

	return true;
}

/*
 * Reset this instance by resetting each FrameState object for each node in [Target] as
 * well as setting [Progress] and [ProgressTicks] to 0.
 */
void AnimationInstance::Reset()
{
	for(unsigned int s = 0; s < StateCount; s++)
	{
		FrameStates[s].Reset();
	}

	Progress = 0;
	ProgressTicks = 0;
}

/*
 * Retrieve the FrameState for a single node in [Target] via its index.
 */
AnimationInstance::FrameState * AnimationInstance::GetFrameState(unsigned int stateIndex)
{
	if(stateIndex >= StateCount)
	{
		Debug::PrintError("AnimationInstance::GetFrameState -> State index is out of bounds.");
		return NULL;
	}

	return FrameStates + stateIndex;
}

void AnimationInstance::SetSpeed(float speedFactor)
{
	this->SpeedFactor = speedFactor;
}

void AnimationInstance::Play()
{
	Playing = true;
	Paused = false;
}

void AnimationInstance::Stop()
{
	Playing = false;
	Reset();
}

void AnimationInstance::Pause()
{
	Paused = true;
}
