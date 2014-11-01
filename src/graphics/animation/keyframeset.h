#ifndef _KEYFRAMESET_H_
#define _KEYFRAMESET_H_

//forward declarations

#include "translationkeyframe.h"
#include "scalekeyframe.h"
#include "rotationkeyframe.h"
#include <vector>

class KeyFrameSet
{
	public:

	std::vector<TranslationKeyFrame> TranslationKeyFrames;
	std::vector<ScaleKeyFrame> ScaleKeyFrames;
	std::vector<RotationKeyFrame> RotationKeyFrames;

	KeyFrameSet();
    ~KeyFrameSet();
};

#endif
