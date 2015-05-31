#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animationinstance.h"
#include "object/enginetypes.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/animation/animation.h"
#include "graphics/animation/skeleton.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <string>

namespace GTE
{
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

		EarlyEndTicks = 0;
		EarlyEnd = 0;

		StartOffsetTicks = 0;
		StartOffset = 0;

		Playing = false;
		Paused = false;

		PlayBackMode = PlaybackMode::Repeat;
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
		if (FrameStates != NULL)
		{
			delete[] FrameStates;
			FrameStates = NULL;
		}

		if (NodeToChannelMap != NULL)
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
	Bool AnimationInstance::Init()
	{
		Destroy();

		NONFATAL_ASSERT_RTRN(Target.IsValid(), "AnimationInstance::Init -> Animation target is invalid.", false, true);
		NONFATAL_ASSERT_RTRN(SourceAnimation.IsValid(), "AnimationInstance::Init -> Animation is invalid.", false, true);

		UInt32 nodeCount = Target->GetNodeCount();
		UInt32 channelCount = SourceAnimation->GetChannelCount();

		if (nodeCount <= 0)return true;

		FrameStates = new FrameState[nodeCount];
		ASSERT(FrameStates != NULL, "AnimationInstance::Init -> Unable to allocate FrameState array.");

		NodeToChannelMap = new int[nodeCount];
		if (NodeToChannelMap == NULL)
		{
			Debug::PrintError("AnimationInstance::Init -> Unable to allocate NodeToChannelMap array.");
			Destroy();
			return false;
		}

		StateCount = nodeCount;

		for (UInt32 n = 0; n < nodeCount; n++)
		{
			SkeletonNode * node = Target->GetNodeFromList(n);
			Int32 foundIndex = -1;

			if (node != NULL)
			{
				for (UInt32 c = 0; c < channelCount; c++)
				{
					const std::string * channelName = SourceAnimation->GetChannelName(c);
					if (channelName == NULL)continue;

					if (node->Name == *channelName)
					{
						foundIndex = c;
					}
				}
			}

			NodeToChannelMap[n] = foundIndex;
		}

		EarlyEndTicks = SourceAnimation->GetEarlyEndTicks();
		EarlyEnd = EarlyEndTicks / SourceAnimation->GetTicksPerSecond();

		StartOffsetTicks = SourceAnimation->GetStartOffsetTicks();
		StartOffset = StartOffsetTicks / SourceAnimation->GetTicksPerSecond();

		DurationTicks = SourceAnimation->GetDurationTicks();
		Duration = DurationTicks / SourceAnimation->GetTicksPerSecond();

		Progress = StartOffset;
		ProgressTicks = StartOffsetTicks;

		return true;
	}

	/*
	 * Reset this instance by resetting each FrameState object for each node in [Target] as
	 * well as setting [Progress] and [ProgressTicks] to 0.
	 */
	void AnimationInstance::Reset()
	{
		for (UInt32 s = 0; s < StateCount; s++)
		{
			FrameStates[s].Reset();
		}

		Progress = StartOffset;
		ProgressTicks = StartOffsetTicks;
	}

	/*
	 * Retrieve the FrameState for a single node in [Target] via its index.
	 */
	AnimationInstance::FrameState * AnimationInstance::GetFrameState(UInt32 stateIndex)
	{
		if (stateIndex >= StateCount)
		{
			Debug::PrintError("AnimationInstance::GetFrameState -> State index is out of bounds.");
			return NULL;
		}

		return FrameStates + stateIndex;
	}

	Int32 AnimationInstance::GetChannelMappingForTargetNode(UInt32 nodeIndex)
	{
		NONFATAL_ASSERT_RTRN(Target.IsValid(), "AnimationInstance::GetChannelMappingForTargetNode -> target skeleton is not valid.", -1, true);
		NONFATAL_ASSERT_RTRN(nodeIndex < Target->GetNodeCount(), "AnimationInstance::GetChannelMappingForTargetNode -> nodeIndex is out of range.", -1, true);

		return NodeToChannelMap[nodeIndex];
	}

	void AnimationInstance::SetSpeed(Real speedFactor)
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
}
