#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blendop.h"
#include "util/time.h"
#include <string>

/*
 * Default constructor.
 */
BlendOp::BlendOp(float duration)
{
	this->duration = duration;
	this->progress = 0;
	complete = false;
	started = false;
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

void BlendOp::Update(std::vector<float>& weights)
{
	progress += Time::GetDeltaTime();
}

bool BlendOp::HasStarted()
{
	return started;
}

void BlendOp::SetStarted(bool started)
{
	this->started = started;
}

void BlendOp::SetComplete(bool complete)
{
	this->complete = complete;
}

bool BlendOp::HasCompleted()
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

