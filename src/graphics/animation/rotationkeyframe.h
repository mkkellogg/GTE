#ifndef _ROTATION_KEYFRAME_H_
#define _ROTATION_KEYFRAME_H_

//forward declarations

#include "keyframe.h"
#include "geometry/quaternion.h"
#include "geometry/matrix4x4.h"

class RotationKeyFrame : public KeyFrame
{
	public:

	Quaternion Rotation;

	RotationKeyFrame();
	RotationKeyFrame(float normalizedTime, float realTime, float realTimeTicks, const Quaternion& rotation);
    ~RotationKeyFrame();
};

#endif
