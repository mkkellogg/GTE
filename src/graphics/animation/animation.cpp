#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/enginetypes.h"
#include "animation.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

Animation::Animation(unsigned int nodeCount, float durationTicks, float ticksPerSecond, SkeletonRef skeleton)
{
	keyFrames = NULL;

	this->nodeCount = nodeCount;
	this->durationTicks = durationTicks;
	this->ticksPerSecond = ticksPerSecond;
	this->skeleton = skeleton;
}

Animation::~Animation()
{
	Destroy();
}

void Animation::Destroy()
{
	if(keyFrames != NULL)
	{
		delete[] keyFrames;
		keyFrames = NULL;
	}
}

bool Animation::Init()
{
	Destroy();

	if(nodeCount == 0)return true;

	keyFrames = new KeyFrameSet[nodeCount];
	NULL_CHECK(keyFrames,"Animation::Init -> Could not allocate key frames parent array", false);

	return true;
}

KeyFrameSet * Animation::GetKeyFrameSet(unsigned int node)
{
	if(node >= nodeCount)
	{
		Debug::PrintError("Animation::GetKeyFrameSet -> Node index is out of range.");
		return NULL;
	}

	return keyFrames + node;
}

float Animation::GetDurationTicks()
{
	return durationTicks;
}

float Animation::GetTicksPerSecond()
{
	return ticksPerSecond;
}

SkeletonRef Animation::GetSkeleton()
{
	return skeleton;
}
