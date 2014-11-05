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

#ifndef _ANIMATION_H_
#define _ANIMATION_H_

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

	// store the number of KeyFrameSet objects that have been allocated, this should
	// match the number of nodes in the target skeleton
	unsigned int keyFrameSetCount;

	// the target skeleton
	SkeletonRef target;

	// the duration of this animation in  device/clock independent ticks
	float durationTicks;
	// map the ticks duration to actual time
	float ticksPerSecond;

	Animation(float durationTicks, float ticksPerSecond, SkeletonRef target);
	~Animation();
	void Destroy();
	bool Init();

	public:

	unsigned int GetKeyFrameSetCount();
	KeyFrameSet * GetKeyFrameSet(unsigned int nodeIndex);
	float GetDurationTicks() const;
	float GetTicksPerSecond() const;
	SkeletonRef GetTarget();
};

#endif
