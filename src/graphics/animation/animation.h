#ifndef _ANIMATION_H_
#define _ANIMATION_H_

//forward declarations

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

	KeyFrameSet * keyFrames;
	unsigned int nodeCount;
	SkeletonRef skeleton;

	float durationTicks;
	float ticksPerSecond;

	Animation(unsigned int nodeCount, float durationTicks, float ticksPerSecond, SkeletonRef skeleton);
	~Animation();
	void Destroy();
	bool Init();

	public:

	KeyFrameSet * GetKeyFrameSet(unsigned int node);
	float GetDurationTicks();
	float GetTicksPerSecond();
	SkeletonRef GetSkeleton();

};

#endif
