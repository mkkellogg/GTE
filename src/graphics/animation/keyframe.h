#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_

//forward declarations

class KeyFrame
{
	public:

	float NormalizedTime;
	float RealTime;

	float RealTimeTicks;

	KeyFrame();
	KeyFrame(float normalizedTime, float realTime, float realTimeTicks);
    ~KeyFrame();
};

#endif
