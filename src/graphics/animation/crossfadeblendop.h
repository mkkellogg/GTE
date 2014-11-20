/*********************************************
*
* class: CrossFadeBlendOp
*
* author: Mark Kellogg
*
* This class is the base class for animation blending operations.
*
***********************************************/

#ifndef _CROSSFADE_BLENDOP_H_
#define _CROSSFADE_BLENDOP_H_

//forward declarations
class Transform;
class SkeletonNode;

#include "blendop.h"
#include "object/enginetypes.h"
#include <vector>
#include <string>
#include <memory>

class CrossFadeBlendOp : public BlendOp
{
	friend class AnimationPlayer;

	unsigned int targetIndex;
	std::function<void(CrossFadeBlendOp*)> startCallback;
	std::function<void(CrossFadeBlendOp*)> completeCallback;
	std::function<void(CrossFadeBlendOp*)> stoppedEarlyCallback;

	CrossFadeBlendOp(float duration, unsigned int targetIndex);

	protected:

	~CrossFadeBlendOp();

	public :

	void Update(std::vector<float>& weights);
	void OnStart();
	void OnComplete();
	void OnStoppedEarly();
	void SetOnStartCallback(std::function<void(CrossFadeBlendOp*)> callback);
	void SetOnCompleteCallback(std::function<void(CrossFadeBlendOp*)> callback);
	void SetOnStoppedEarlyCallback(std::function<void(CrossFadeBlendOp*)> callback);
	unsigned int GetTargetIndex();
};

#endif
