#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animation.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

Animation::Animation(unsigned int boneCount, float duration, float ticksPerSecond)
{
	keyFrames = NULL;
	this->boneCount = boneCount;
	this->duration = duration;
	this->ticksPerSecond = ticksPerSecond;
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

	if(boneCount == 0)return true;

	keyFrames = new KeyFrameSet[boneCount];
	NULL_CHECK(keyFrames,"Animation::Init -> Could not allocate key frames parent array", false);

	return true;
}

KeyFrameSet * Animation::GetKeyFrameSet(unsigned int bone)
{
	if(bone >= boneCount)
	{
		Debug::PrintError("Animation::GetKeyFrameSet -> Bone index is out of range.");
		return NULL;
	}

	return keyFrames + bone;
}

float Animation::GetDuration()
{
	return duration;
}

float Animation::GetTicksPerSecond()
{
	return ticksPerSecond;
}
