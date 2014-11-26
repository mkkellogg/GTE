#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/enginetypes.h"
#include "animation.h"
#include "skeleton.h"
#include "global/global.h"
#include "ui/debug.h"
#include <string>

/*
 * Single constructor, which initializes all member variables of this animation.
 */
Animation::Animation(float durationTicks, float ticksPerSecond, SkeletonRef target)
{
	keyFrames = NULL;
	this->durationTicks = durationTicks;
	this->ticksPerSecond = ticksPerSecond;
	this->target = target;
	keyFrameSetCount = 0;
}

/*
 * Cleanup the animation
 */
Animation::~Animation()
{
	Destroy();
}

/*
 * This method destroys the member array of KeyFrameSet objects and invalidates the array pointer.
 */
void Animation::Destroy()
{
	if(keyFrames != NULL)
	{
		delete[] keyFrames;
		keyFrames = NULL;
	}
	keyFrameSetCount = 0;
}

/*
 * Initialize this animation. This method will validate the target skeleton [target] and allocate
 * a KeyFrameSet object for each node in [target] in [keyframes].
 */
bool Animation::Init()
{
	Destroy();

	ASSERT(target.IsValid(),"Animation::Init -> Animation target is not valid.",false);
	if(target->GetNodeCount() == 0)return true;

	keyFrames = new KeyFrameSet[target->GetNodeCount()];
	ASSERT(keyFrames != NULL,"Animation::Init -> Could not allocate key frame set array", false);

	keyFrameSetCount = target->GetNodeCount();

	return true;
}

/*
 * Return the number of KeyFrameSet objects in [keyFrames].
 */
unsigned int Animation::GetKeyFrameSetCount()
{
	return keyFrameSetCount;
}

/*
 * Retrieve the KeyFrameSet corresponding to a particular node in [target].
 */
KeyFrameSet * Animation::GetKeyFrameSet(unsigned int nodeIndex)
{
	ASSERT(nodeIndex < keyFrameSetCount,"Animation::GetKeyFrameSet -> Node index is out of range.", NULL);
	return keyFrames + nodeIndex;
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
 * Get a reference to the target of this animation.
 */
SkeletonRef Animation::GetTarget()
{
	return target;
}
