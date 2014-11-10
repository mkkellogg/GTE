#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crossfadeblendop.h"
#include <string>
#include "global/global.h"
#include "ui/debug.h"
#include "util/time.h"

/*
 * Default constructor.
 */
CrossFadeBlendOp::CrossFadeBlendOp(float duration, unsigned int targetIndex) : BlendOp(duration)
{
	this->targetIndex = targetIndex;
	SetComplete(false);
	if(duration <=0)SetComplete(true);
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

	float normalizedProgress = progress / duration;
	for(unsigned int i = 0; i < initialWeights.size(); i++)
	{
		float initialWeight = initialWeights[i];
		float targetWeight = 0;
		if(i == targetIndex)targetWeight = 1;
		float deltaWeight = targetWeight - initialWeight;
		weights[i] = normalizedProgress * deltaWeight + initialWeight;
	}

	progress += Time::GetDeltaTime();
	if(progress > duration)
	{
		for(unsigned int i = 0; i < initialWeights.size(); i++)
		{
			if(i == targetIndex)weights[i] = 1;
			else weights[i] = 0;
		}
		SetComplete(true);
	}
}

