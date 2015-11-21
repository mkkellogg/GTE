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

#include "engine.h"
#include "global/global.h"
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
		ScaleKeyFrame(Real normalizedTime, Real realTime, Real realTimeTicks, const Vector3& scale);
		~ScaleKeyFrame() override;
	};

}
#endif
