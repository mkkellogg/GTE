#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crossfadeblendop.h"
#include <string>
#include "global/global.h"
#include "ui/debug.h"
#include "util/time.h"
#include <memory>

/*
 * Default constructor.
 */
CrossFadeBlendOp::CrossFadeBlendOp(float duration, unsigned int targetIndex) : BlendOp(duration)
{
	this->targetIndex = targetIndex;
	NotifyComplete(false);
	if(duration <=0)NotifyComplete(true);
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
void CrossFadeBlendOp::Update(std::vector<float>& weights)
{
	if(complete)return;
	ASSERT_RTRN(weights.size() >= initialWeights.size(), "CrossFadeBlendOp::Update -> Weight count is less than initial weight count.");

	BlendOp::Update(weights);
	float normalizedProgress = progress / duration;

	for(unsigned int i = 0; i < initialWeights.size(); i++)
	{
		float initialWeight = initialWeights[i];
		float targetWeight = 0;
		if(i == targetIndex)targetWeight = 1;
		float deltaWeight = targetWeight - initialWeight;
		weights[i] = normalizedProgress * deltaWeight + initialWeight;
	}

	if(progress > duration)
	{
		for(unsigned int i = 0; i < initialWeights.size(); i++)
		{
			if(i == targetIndex)weights[i] = 1;
			else weights[i] = 0;
		}
		NotifyComplete(true);
	}
}

void CrossFadeBlendOp::OnStart()
{
	if(startCallback)startCallback(this);
}

void CrossFadeBlendOp::OnComplete()
{
	if(completeCallback)completeCallback(this);
}

void CrossFadeBlendOp::SetOnStartCallback(std::function<void(CrossFadeBlendOp*)> callback)
{
	this->startCallback = callback;
}

void CrossFadeBlendOp::SetOnCompleteCallback(std::function<void(CrossFadeBlendOp*)> callback)
{
	this->completeCallback = callback;
}

