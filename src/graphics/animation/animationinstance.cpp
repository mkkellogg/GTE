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

	NodeToChannelMap = NULL;

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
	Destroy();
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

	if(NodeToChannelMap != NULL)
	{
		delete[] NodeToChannelMap;
		NodeToChannelMap = NULL;
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
	Destroy();

	ASSERT(Target.IsValid(), "AnimationInstance::Init -> Animation target is invalid.", false);
	ASSERT(SourceAnimation.IsValid(), "AnimationInstance::Init -> Animation is invalid.", false);

	unsigned int nodeCount = Target->GetNodeCount();
	unsigned int channelCount = SourceAnimation->GetChannelCount();

	if(nodeCount <= 0)return true;

	FrameStates = new FrameState[nodeCount];
	ASSERT(FrameStates != NULL, "AnimationInstance::Init -> Unable to allocate FrameState array.", false);

	NodeToChannelMap = new int[nodeCount];
	if(NodeToChannelMap == NULL)
	{
		Debug::PrintError("AnimationInstance::Init -> Unable to allocate NodeToChannelMap array.");
		Destroy();
		return false;
	}

	StateCount = nodeCount;

	for(unsigned int n = 0; n < nodeCount; n++)
	{
		SkeletonNode * node = Target->GetNodeFromList(n);
		int foundIndex = -1;

		if(node != NULL)
		{
			for(unsigned int c = 0; c < channelCount; c++)
			{
				const std::string * channelName = SourceAnimation->GetChannelName(c);
				if(channelName == NULL)continue;

				if(node->Name == *channelName)
				{
					foundIndex = c;
				}
			}
		}

		NodeToChannelMap[n] = foundIndex;
	}

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

int AnimationInstance::GetChannelMappingForTargetNode(unsigned int nodeIndex)
{
	ASSERT(Target.IsValid(), "AnimationInstance::GetChannelMappingForTargetNode -> target skeleton is not valid.", -1);
	ASSERT(nodeIndex < Target->GetNodeCount(), "AnimationInstance::GetChannelMappingForTargetNode -> nodeIndex is out of range.", -1);

	return NodeToChannelMap[nodeIndex];
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
