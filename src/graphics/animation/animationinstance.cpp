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
	this->target = target;
	this->animation = animation;

	stateCount = 0;
	frameStates = NULL;

	duration = 0;
	startTime = 0;
	lastTime = 0;
}

AnimationInstance::~AnimationInstance()
{

}

void AnimationInstance::Destroy()
{
	if(frameStates != NULL)
	{
		delete[] frameStates;
		frameStates = NULL;
	}
}

bool AnimationInstance::Init()
{
	SHARED_REF_CHECK(target, "AnimationInstance::Init -> Animation target is invalid.", false);
	SHARED_REF_CHECK(animation, "AnimationInstance::Init -> Animation is invalid.", false);

	Destroy();

	unsigned int nodeCount = target->GetNodeCount();
	if(nodeCount <= 0)return true;

	frameStates = new FrameState[nodeCount];
	for(unsigned int n = 0; n < nodeCount; n++)
	{
		FrameState state;
		frameStates[n] = state;
	}
	stateCount = nodeCount;

	return true;
}

AnimationInstance::FrameState * AnimationInstance::GetFrameState(unsigned int nodeIndex)
{
	if(nodeIndex >= stateCount)
	{
		Debug::PrintError("AnimationInstance::GetFrameState -> Node index is out of bounds.");
		return NULL;
	}

	return frameStates + nodeIndex;
}

