/*********************************************
*
* class: BlendOp
*
* author: Mark Kellogg
*
* This class is the base class for animation blending operations.
*
***********************************************/

#ifndef _BLENDOP_H_
#define _BLENDOP_H_

//forward declarations
class Transform;
class SkeletonNode;

#include <vector>
#include <string>
#include "object/enginetypes.h"

class BlendOp
{
	friend class AnimationPlayer;

	protected:

	float duration;
	float progress;
	bool complete;

	std::vector<float> initialWeights;

	BlendOp(float duration);
	virtual ~BlendOp();
	void SetComplete(bool complete);

	public :

	void Reset();
	bool Init(std::vector<float>& initialWeights);
	virtual void Update(std::vector<float>& weights) = 0;
	bool IsComplete();
	float GetNormalizedProgress();
	float GetProgress();

};

#endif
