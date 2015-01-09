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
#include <vector>

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
	unsigned int channelCount;

	// the duration of this animation in  device/clock independent ticks
	float durationTicks;
	// map the ticks duration to actual time
	float ticksPerSecond;

	Animation(float durationTicks, float ticksPerSecond);
	~Animation();
	void Destroy();
	bool Init(unsigned int channelCount);

	public:

	unsigned int GetChannelCount();
	KeyFrameSet * GetKeyFrameSet(unsigned int nodeIndex);
	const std::string * GetChannelName(unsigned int index);
	void SetChannelName(unsigned int index, const std::string& name);
	float GetDurationTicks() const;
	float GetTicksPerSecond() const;
};

#endif
