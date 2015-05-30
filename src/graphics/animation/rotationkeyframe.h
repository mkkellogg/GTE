/*********************************************
*
* class: RotationKeyFrame
*
* author: Mark Kellogg
*
* This class encapsulates a single rotation key frame.
*
***********************************************/

#ifndef _GTE_ROTATION_KEYFRAME_H_
#define _GTE_ROTATION_KEYFRAME_H_

#include "keyframe.h"
#include "geometry/quaternion.h"
#include "geometry/matrix4x4.h"

namespace GTE
{
	class RotationKeyFrame : public KeyFrame
	{
	public:

		// The rotation transformation
		Quaternion Rotation;

		RotationKeyFrame();
		RotationKeyFrame(float normalizedTime, float realTime, float realTimeTicks, const Quaternion& rotation);
		~RotationKeyFrame();
	};
}

#endif
