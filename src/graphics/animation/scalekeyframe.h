#ifndef _SCALE_KEYFRAME_H_
#define _SCALE_KEYFRAME_H_

//forward declarations

#include "keyframe.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

class ScaleKeyFrame : public KeyFrame
{
	public:

	Vector3 Scale;

	ScaleKeyFrame();
	ScaleKeyFrame(float normalizedTime, float realTime, const Vector3& scale);
    ~ScaleKeyFrame();
};

#endif
