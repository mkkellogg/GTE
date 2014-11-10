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

class CrossFadeBlendOp : public BlendOp
{
	friend class AnimationPlayer;

	unsigned int targetIndex;

	CrossFadeBlendOp(float duration, unsigned int targetIndex);

	protected:

	~CrossFadeBlendOp();

	public :

	void Update(std::vector<float>& weights);
};

#endif
