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

class KeyFrame
{
	public:

	// the time in the animation of this key frame, in seconds
	float RealTime;
	// the time of this key frame in the animation, normalized to the range 0-1
	float NormalizedTime;
	// the time of this key frame in the animation in ticks
	float RealTimeTicks;

	KeyFrame();
	KeyFrame(float normalizedTime, float realTime, float realTimeTicks);
    ~KeyFrame();
};

#endif
