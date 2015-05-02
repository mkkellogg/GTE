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
 * Simple constructor, calls the main constructor with [startOffsetTicks] = 0 && [earlyEndTicks] = [durationTicks]
 */
Animation::Animation(float durationTicks, float ticksPerSecond) : Animation(durationTicks, ticksPerSecond, 0, durationTicks)
{

}

/*
 * Main constructor - initializes all member variables of this animation.
 */
Animation::Animation(float durationTicks, float ticksPerSecond, float startOffsetTicks, float earlyEndTicks)
{
	//force ticksPerSecond > 0
	if(ticksPerSecond <=0)ticksPerSecond = 1;

	keyFrames = NULL;
	this->durationTicks = durationTicks;
	this->ticksPerSecond = ticksPerSecond;
	this->startOffsetTicks = startOffsetTicks;
	this->earlyEndTicks = earlyEndTicks;
	channelCount = 0;
	channelNames = NULL;

	if(this->durationTicks <0)this->durationTicks = 0;
	if(this->startOffsetTicks < 0)this->startOffsetTicks = 0;
	if(this->earlyEndTicks < 0)this->earlyEndTicks = 0;

	if(this->earlyEndTicks > this->durationTicks)this->earlyEndTicks = this->durationTicks;
	if(this->startOffsetTicks > this->earlyEndTicks)this->startOffsetTicks = this->earlyEndTicks;
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
	ASSERT(keyFrames != NULL,"Animation::Init -> Could not allocate key frame set array.");

	channelNames = new std::string[channelCount];
	ASSERT(channelNames != NULL,"Animation::Init -> Could not allocate channel name set array.");

	return true;
}

/*
 * Clip the beginning [startOffset] seconds from the animation, and the end
 * [earlyEnd] seconds from the animation.
 */
void Animation::ClipEnds(float startOffset, float earlyEnd)
{
	this->startOffsetTicks = startOffset * ticksPerSecond;
	this->earlyEndTicks = earlyEnd * ticksPerSecond;

	if(this->earlyEndTicks < 0) this->earlyEndTicks = 0;
	if(this->startOffsetTicks < 0) this->startOffsetTicks = 0;
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
	NONFATAL_ASSERT_RTRN(nodeIndex < channelCount,"Animation::GetKeyFrameSet -> Node index is out of range.", NULL, true);
	return keyFrames + nodeIndex;
}

const std::string * Animation::GetChannelName(unsigned int index)
{
	NONFATAL_ASSERT_RTRN(index < channelCount, "Animation::GetChannelName -> 'index' is out of range.", NULL, true);
	return channelNames + index;
}

void Animation::SetChannelName(unsigned int index, const std::string& name)
{
	NONFATAL_ASSERT(index < channelCount, "Animation::SetChannelName -> 'index' is out of range.", true);
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

/*
 * Get the start offset of this animation in ticks.
 */
float Animation::GetStartOffsetTicks() const
{
	return startOffsetTicks;
}

/*
 * Get the point where this animation ends relative to its full duration in ticks.
 */
float Animation::GetEarlyEndTicks() const
{
	return earlyEndTicks;
}

/*
 * Get the duration of this animation in seconds.
 */
float Animation::GetDuration() const
{
	return durationTicks/ticksPerSecond;
}

/*
 * Get the start offset of this animation in seconds.
 */
float Animation::GetStartOffset() const
{
	return startOffsetTicks/ticksPerSecond;
}

/*
 * Get the point where this animation ends relative to its full duration in seconds.
 */
float Animation::GetEarlyEnd() const
{
	return earlyEndTicks/ticksPerSecond;
}
