#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_

//forward declarations

class KeyFrame
{
	public:

	float NormalizedTime;
	float RealTime;

	KeyFrame();
	KeyFrame(float normalizedTime, float realTime);
    ~KeyFrame();
};

#endif
