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

#include "engine.h"
#include "global/global.h"
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
		RotationKeyFrame(Real normalizedTime, Real realTime, Real realTimeTicks, const Quaternion& rotation);
		~RotationKeyFrame() override;
	};
}

#endif
