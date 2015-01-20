#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/enginetypes.h"
#include "animation.h"
#include "skeleton.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <string>

/*
 * Single constructor, which initializes all member variables of this animation.
 */
Animation::Animation(float durationTicks, float ticksPerSecond)
{
	keyFrames = NULL;
	this->durationTicks = durationTicks;
	this->ticksPerSecond = ticksPerSecond;
	channelCount = 0;
	channelNames = NULL;
}

/*
 * Cleanup the animation
 */
Animation::~Animation()
{
	Destroy();
}

/*
 * This method destroys [channelNames] and [keyFrames] and invalidates their pointers
 */
void Animation::Destroy()
{
	if(keyFrames != NULL)
	{
		delete[] keyFrames;
		keyFrames = NULL;
	}

	if(channelNames != NULL)
	{
		delete[] channelNames;
		channelNames = NULL;
	}

	channelCount = 0;
}

/*
 * Initialize this animation. This method will allocate [keyFrameSetCount]] key frame sets.
 */
bool Animation::Init(unsigned int channelCount)
{
	Destroy();

	this->channelCount = channelCount;

	keyFrames = new KeyFrameSet[channelCount];
	ASSERT(keyFrames != NULL,"Animation::Init -> Could not allocate key frame set array", false);

	channelNames = new std::string[channelCount];
	if(channelNames == NULL)
	{
		Destroy();
		Debug::PrintError("Animation::Init -> Could not allocate channel name set array");
		return false;
	}

	return true;
}

/*
 * Return the number of KeyFrameSet objects in [keyFrames].
 */
unsigned int Animation::GetChannelCount()
{
	return channelCount;
}

/*
 * Retrieve the KeyFrameSet corresponding to a particular node in [target].
 */
KeyFrameSet * Animation::GetKeyFrameSet(unsigned int nodeIndex)
{
	ASSERT(nodeIndex < channelCount,"Animation::GetKeyFrameSet -> Node index is out of range.", NULL);
	return keyFrames + nodeIndex;
}

const std::string * Animation::GetChannelName(unsigned int index)
{
	ASSERT(index < channelCount, "Animation::GetChannelName -> index is out of range.", NULL);
	return channelNames + index;
}

void Animation::SetChannelName(unsigned int index, const std::string& name)
{
	ASSERT_RTRN(index < channelCount, "Animation::SetChannelName -> index is out of range.");
	channelNames[index] = name;
}

/*
 * Get the duration of this animation in ticks.
 */
float Animation::GetDurationTicks() const
{
	return durationTicks;
}

/*
 * Get the mapping of duration ticks to seconds.
 */
float Animation::GetTicksPerSecond() const
{
	return ticksPerSecond;
}
