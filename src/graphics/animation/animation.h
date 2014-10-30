#ifndef _ANIMATION_H_
#define _ANIMATION_H_

//forward declarations

#include "object/engineobject.h"
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
	unsigned int boneCount;

	float duration;
	float ticksPerSecond;

	Animation(unsigned int boneCount, float duration, float ticksPerSecond);
	~Animation();
	void Destroy();
	bool Init();

	public:

	KeyFrameSet * GetKeyFrameSet(unsigned int bone);
	float GetDuration();
	float GetTicksPerSecond();

};

#endif
