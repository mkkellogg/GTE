#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object/enginetypes.h"
#include "animation.h"
#include "skeleton.h"
#include "global/global.h"
#include "debug/gtedebug.h"
#include <string>

namespace GTE
{
	/*
* Simple constructor, calls the main constructor with [startOffsetTicks] = 0 && [earlyEndTicks] = [durationTicks]
*/
	Animation::Animation(Real durationTicks, Real ticksPerSecond) : Animation(durationTicks, ticksPerSecond, 0, durationTicks)
	{

	}

	/*
	 * Main constructor - initializes all member variables of this animation.
	 */
	Animation::Animation(Real durationTicks, Real ticksPerSecond, Real startOffsetTicks, Real earlyEndTicks)
	{
		//force ticksPerSecond > 0
		if (ticksPerSecond <= 0)ticksPerSecond = 1;

		keyFrames = nullptr;
		this->durationTicks = durationTicks;
		this->ticksPerSecond = ticksPerSecond;
		this->startOffsetTicks = startOffsetTicks;
		this->earlyEndTicks = earlyEndTicks;
		channelCount = 0;
		channelNames = nullptr;

		if (this->durationTicks < 0)this->durationTicks = 0;
		if (this->startOffsetTicks < 0)this->startOffsetTicks = 0;
		if (this->earlyEndTicks < 0)this->earlyEndTicks = 0;

		if (this->earlyEndTicks > this->durationTicks)this->earlyEndTicks = this->durationTicks;
		if (this->startOffsetTicks > this->earlyEndTicks)this->startOffsetTicks = this->earlyEndTicks;
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
		if (keyFrames != nullptr)
		{
			delete[] keyFrames;
			keyFrames = nullptr;
		}

		if (channelNames != nullptr)
		{
			delete[] channelNames;
			channelNames = nullptr;
		}

		channelCount = 0;
	}

	/*
	 * Initialize this animation. This method will allocate [keyFrameSetCount]] key frame sets.
	 */
	Bool Animation::Init(UInt32 channelCount)
	{
		Destroy();

		this->channelCount = channelCount;

		keyFrames = new(std::nothrow) KeyFrameSet[channelCount];
		ASSERT(keyFrames != nullptr, "Animation::Init -> Could not allocate key frame set array.");

		channelNames = new(std::nothrow) std::string[channelCount];
		ASSERT(channelNames != nullptr, "Animation::Init -> Could not allocate channel name set array.");

		return true;
	}

	/*
	 * Clip the beginning [startOffset] seconds from the animation, and the end
	 * [earlyEnd] seconds from the animation.
	 */
	void Animation::ClipEnds(Real startOffset, Real earlyEnd)
	{
		this->startOffsetTicks = startOffset * ticksPerSecond;
		this->earlyEndTicks = earlyEnd * ticksPerSecond;

		if (this->earlyEndTicks < 0) this->earlyEndTicks = 0;
		if (this->startOffsetTicks < 0) this->startOffsetTicks = 0;
	}

	/*
	 * Return the number of KeyFrameSet objects in [keyFrames].
	 */
	UInt32 Animation::GetChannelCount() const
	{
		return channelCount;
	}

	/*
	 * Retrieve the KeyFrameSet corresponding to a particular node in [target].
	 */
	KeyFrameSet * Animation::GetKeyFrameSet(UInt32 nodeIndex)
	{
		NONFATAL_ASSERT_RTRN(nodeIndex < channelCount, "Animation::GetKeyFrameSet -> Node index is out of range.", nullptr, true);
		return keyFrames + nodeIndex;
	}

	const std::string * Animation::GetChannelName(UInt32 index) const
	{
		NONFATAL_ASSERT_RTRN(index < channelCount, "Animation::GetChannelName -> 'index' is out of range.", nullptr, true);
		return channelNames + index;
	}

	void Animation::SetChannelName(UInt32 index, const std::string& name)
	{
		NONFATAL_ASSERT(index < channelCount, "Animation::SetChannelName -> 'index' is out of range.", true);
		channelNames[index] = name;
	}

	/*
	 * Get the duration of this animation in ticks.
	 */
	Real Animation::GetDurationTicks() const
	{
		return durationTicks;
	}

	/*
	 * Get the mapping of duration ticks to seconds.
	 */
	Real Animation::GetTicksPerSecond() const
	{
		return ticksPerSecond;
	}

	/*
	 * Get the start offset of this animation in ticks.
	 */
	Real Animation::GetStartOffsetTicks() const
	{
		return startOffsetTicks;
	}

	/*
	 * Get the point where this animation ends relative to its full duration in ticks.
	 */
	Real Animation::GetEarlyEndTicks() const
	{
		return earlyEndTicks;
	}

	/*
	 * Get the duration of this animation in seconds.
	 */
	Real Animation::GetDuration() const
	{
		return durationTicks / ticksPerSecond;
	}

	/*
	 * Get the start offset of this animation in seconds.
	 */
	Real Animation::GetStartOffset() const
	{
		return startOffsetTicks / ticksPerSecond;
	}

	/*
	 * Get the point where this animation ends relative to its full duration in seconds.
	 */
	Real Animation::GetEarlyEnd() const
	{
		return earlyEndTicks / ticksPerSecond;
	}
}
