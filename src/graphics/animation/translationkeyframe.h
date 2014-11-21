/*********************************************
*
* class: TranslationKeyFrame
*
* author: Mark Kellogg
*
* This class encapsulates a single translation key frame.
*
***********************************************/

#ifndef _TRANSLATION_KEYFRAME_H_
#define _TRANSLATION_KEYFRAME_H_

#include "keyframe.h"
#include "geometry/vector/vector3.h"
#include "geometry/matrix4x4.h"

class TranslationKeyFrame : public KeyFrame
{
	public:

	// translation transformation
	Vector3 Translation;

	TranslationKeyFrame();
	TranslationKeyFrame(float normalizedTime, float realTime, float realTimeTicks, const Vector3& translation);
    ~TranslationKeyFrame();
};

#endif