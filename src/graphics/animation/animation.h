/*********************************************
*
* class: Animation
*
* author: Mark Kellogg
*
* This class encapsulates key frame sequences for all nodes
* in a Skeleton object.
*
***********************************************/

#ifndef _GTE_ANIMATION_H_
#define _GTE_ANIMATION_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "keyframeset.h"
#include "geometry/vector/vector3.h"
#include "geometry/quaternion.h"
#include "geometry/matrix4x4.h"
#include "global/global.h"
#include <vector>

namespace GTE
{
	class Animation : public EngineObject
	{
		friend class EngineObjectManager;
		friend class ModelImporter;

		// A KeyFrameSet for each node in the target skeleton
		KeyFrameSet * keyFrames;

		// the channel name of each key frame set, used in animation targeting
		// 1:1 correspondence with [keyFrames]
		std::string * channelNames;

		// store the number of KeyFrameSet objects that have been allocated, which
		// is also the length of [channelNames]
		UInt32 channelCount;

		// the duration of this animation in  device/clock independent ticks
		Real durationTicks;
		// map the ticks duration to actual time
		Real ticksPerSecond;

		// we can have the animation start with progress > 0
		Real startOffsetTicks;

		// we can have the animation end earlier than [durationTicks]
		Real earlyEndTicks;

		Animation(Real durationTicks, Real ticksPerSecond);
		Animation(Real durationTicks, Real ticksPerSecond, Real startOffsetTicks, Real earlyEndTicks);
		~Animation();
		void Destroy();
		Bool Init(UInt32 channelCount);

	public:

		void ClipEnds(Real startOffsetTicks, Real earlyEndTicks);
		UInt32 GetChannelCount() const;
		KeyFrameSet * GetKeyFrameSet(UInt32 nodeIndex);
		const std::string * GetChannelName(UInt32 index) const;
		void SetChannelName(UInt32 index, const std::string& name);
		Real GetTicksPerSecond() const;
		Real GetDurationTicks() const;
		Real GetStartOffsetTicks() const;
		Real GetEarlyEndTicks() const;
		Real GetDuration() const;
		Real GetStartOffset() const;
		Real GetEarlyEnd() const;
	};
}

#endif
