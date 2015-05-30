/*********************************************
*
* class: ScaleKeyFrame
*
* author: Mark Kellogg
*
* This class encapsulates a single scale key frame.
*
***********************************************/

#ifndef _GTE_SCALE_KEYFRAME_H_
#define _GTE_SCALE_KEYFRAME_H_

#include "keyframe.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

namespace GTE
{
	class ScaleKeyFrame : public KeyFrame
	{
	public:

		// scale transformation
		Vector3 Scale;

		ScaleKeyFrame();
		ScaleKeyFrame(float normalizedTime, float realTime, float realTimeTicks, const Vector3& scale);
		~ScaleKeyFrame();
	};

}
#endif
