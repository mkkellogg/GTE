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

bool BlendOp::IsStarted()
{
	return started;
}

void BlendOp::SignalStarted()
{
	started = true;
}

void BlendOp::NotifyComplete(bool complete)
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

