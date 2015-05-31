/*********************************************
*
* class: KeyFrame
*
* author: Mark Kellogg
*
* A single key frame in an animation. This class serves as the
* base class for TranslationKeyFrame, ScaleKeyFrame, and RotationKeyFrame.
*
***********************************************/

#ifndef _GTE_KEYFRAME_H_
#define _GTE_KEYFRAME_H_

#include "global/global.h"

namespace GTE
{
	class KeyFrame
	{
	public:

		// the time in the animation of this key frame, in seconds
		Real RealTime;
		// the time of this key frame in the animation, normalized to the range 0-1
		Real NormalizedTime;
		// the time of this key frame in the animation in ticks
		Real RealTimeTicks;

		KeyFrame();
		KeyFrame(Real normalizedTime, Real realTime, Real realTimeTicks);
		~KeyFrame();
	};
}

#endif
