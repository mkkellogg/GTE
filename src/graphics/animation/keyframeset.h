/*********************************************
*
* class: KeyFrameSet
*
* author: Mark Kellogg
*
* This class encapsulates all the key frames in an animation for a single node/transform.
*
***********************************************/

#ifndef _GTE_KEYFRAMESET_H_
#define _GTE_KEYFRAMESET_H_

#include <vector>

#include "engine.h"
#include "translationkeyframe.h"
#include "scalekeyframe.h"
#include "rotationkeyframe.h"
#include "global/global.h"

namespace GTE
{
	class KeyFrameSet
	{
	public:

		// is this key frame set active?
		Bool Used;
		// key frames with translation transformations
		std::vector<TranslationKeyFrame> TranslationKeyFrames;
		// key frames with scale transformations
		std::vector<ScaleKeyFrame> ScaleKeyFrames;
		// key frames with rotation transformations
		std::vector<RotationKeyFrame> RotationKeyFrames;

		KeyFrameSet();
		~KeyFrameSet();
	};
}

#endif
