#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crossfadeblendop.h"
#include <string>
#include "global/global.h"
#include "global/assert.h"
#include "debug/gtedebug.h"
#include "util/time.h"
#include "engine.h"
#include <memory>

namespace GTE
{
	/*
* Default constructor.
*/
	CrossFadeBlendOp::CrossFadeBlendOp(Real duration, UInt32 targetIndex) : BlendOp(duration)
	{
		this->targetIndex = targetIndex;
		SetComplete(false);
		if (duration <= 0)SetComplete(true);
	}

	/*
	 * Destructor.
	 */
	CrossFadeBlendOp::~CrossFadeBlendOp()
	{

	}

	/*
	 *Adjust the relevant weights of animation player.
	 */
	void CrossFadeBlendOp::Update(std::vector<Real>& weights)
	{
		if (complete)return;
		NONFATAL_ASSERT(weights.size() >= initialWeights.size(), "CrossFadeBlendOp::Update -> Weight count is less than initial weight count.", true);

		BlendOp::Update(weights);
		Real normalizedProgress = progress / duration;

		for (UInt32 i = 0; i < initialWeights.size(); i++)
		{
			Real initialWeight = initialWeights[i];
			Real targetWeight = 0;
			if (i == targetIndex)targetWeight = 1;
			Real deltaWeight = targetWeight - initialWeight;
			weights[i] = normalizedProgress * deltaWeight + initialWeight;
		}

		if (progress > duration)
		{
			for (UInt32 i = 0; i < initialWeights.size(); i++)
			{
				if (i == targetIndex)weights[i] = 1;
				else weights[i] = 0;
			}
			SetComplete(true);
		}
	}

	void CrossFadeBlendOp::OnStart()
	{
		if (startCallback)startCallback(this);
	}

	void CrossFadeBlendOp::OnComplete()
	{
		if (completeCallback)completeCallback(this);
	}

	void CrossFadeBlendOp::OnStoppedEarly()
	{
		if (stoppedEarlyCallback)stoppedEarlyCallback(this);
	}

	void CrossFadeBlendOp::SetOnStartCallback(std::function<void(CrossFadeBlendOp*)> callback)
	{
		this->startCallback = callback;
	}

	void CrossFadeBlendOp::SetOnCompleteCallback(std::function<void(CrossFadeBlendOp*)> callback)
	{
		this->completeCallback = callback;
	}

	void CrossFadeBlendOp::SetOnStoppedEarlyCallback(std::function<void(CrossFadeBlendOp*)> callback)
	{
		this->stoppedEarlyCallback = callback;
	}
	/*
	 * Get the target animation index for this cross fade operation.
	 */
	UInt32 CrossFadeBlendOp::GetTargetIndex() const
	{
		return targetIndex;
	}
}

