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

AnimationInstance::AnimationInstance(SkeletonRef target, AnimationRef animation)
{
	this->Target = target;
	this->Animation = animation;

	StateCount = 0;
	FrameStates = NULL;

	Duration = 0;
	Progress = 0;

	DurationTicks = 0L;
	ProgressTicks = 0L;

	Weight = 1;
	Playing = false;
}

AnimationInstance::~AnimationInstance()
{

}

void AnimationInstance::Destroy()
{
	if(FrameStates != NULL)
	{
		delete[] FrameStates;
		FrameStates = NULL;
	}
}

bool AnimationInstance::Init()
{
	SHARED_REF_CHECK(Target, "AnimationInstance::Init -> Animation target is invalid.", false);
	SHARED_REF_CHECK(Animation, "AnimationInstance::Init -> Animation is invalid.", false);

	Destroy();

	unsigned int nodeCount = Target->GetNodeCount();
	if(nodeCount <= 0)return true;

	FrameStates = new FrameState[nodeCount];
	for(unsigned int n = 0; n < nodeCount; n++)
	{
		FrameState state;
		FrameStates[n] = state;
	}
	StateCount = nodeCount;

	DurationTicks = Animation->GetDurationTicks();
	Duration = DurationTicks / Animation->GetTicksPerSecond();

	Progress = 0;
	ProgressTicks = 0L;

	return true;
}

void AnimationInstance::Reset()
{
	for(unsigned int s = 0; s < StateCount; s++)
	{
		FrameStates[s].Reset();
	}

	Progress = 0;
	ProgressTicks = 0L;
}

AnimationInstance::FrameState * AnimationInstance::GetFrameState(unsigned int stateIndex)
{
	if(stateIndex >= StateCount)
	{
		Debug::PrintError("AnimationInstance::GetFrameState -> State index is out of bounds.");
		return NULL;
	}

	return FrameStates + stateIndex;
}

bool AnimationInstance::IsPlaying()
{
	return Playing;
}

void AnimationInstance::Play()
{
	Playing = true;
}

void AnimationInstance::Stop()
{
	Playing = false;
	Reset();
}

void AnimationInstance::Pause()
{
	Playing = false;
}
