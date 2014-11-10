#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blendop.h"
#include <string>

/*
 * Default constructor.
 */
BlendOp::BlendOp(float duration)
{
	this->duration = duration;
	this->progress = 0;
	complete = false;
}

/*
 * Destructor.
 */
BlendOp::~BlendOp()
{

}

bool BlendOp::Init(std::vector<float>& initialWeights)
{
	this->initialWeights = initialWeights;
	Reset();
	return true;
}

void BlendOp::Reset()
{
	progress = 0;
	complete = false;
}

void BlendOp::SetComplete(bool complete)
{
	this->complete = complete;
}

bool BlendOp::IsComplete()
{
	return complete;
}

float BlendOp::GetNormalizedProgress()
{
	return progress / duration;
}

float BlendOp::GetProgress()
{
	return progress;
}

